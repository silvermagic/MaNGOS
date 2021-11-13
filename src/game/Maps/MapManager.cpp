/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include "Utilities/Callback.h"
#include "Utilities/SingletonImpl.h"
#include "MapManager.h"
#include "SQLStorages.h"
#include "World.h"

#define CLASS_LOCK MaNGOS::ClassLevelLockable<MapManager, boost::recursive_mutex>
INSTANTIATE_SINGLETON_2(MapManager, CLASS_LOCK);
INSTANTIATE_CLASS_MUTEX(MapManager, boost::recursive_mutex);

#define MAX_MAP_LOADERS 4

class LoaderResult // ������صĵ�ͼ����
{
public:
	std::vector<uint8> data;
};

class ILoaderCallBack // ��ͼ���ػص������ӿ�
{
public:
	virtual void Execute() = 0;
	virtual ~ILoaderCallBack() {}
	virtual void SetResult(LoaderResult *result) = 0; // ��Ҫ������ͼ���ݼ����������߳���ִ����ɺ�ͨ���˽ӿڴ����ȡ��������
};

template<class CB>
class _ILoaderCallBack : public CB, public ILoaderCallBack // �����Ҫ����CB���ĵ�һ��������������LoaderResult*
{
public:
	_ILoaderCallBack(CB const& cb) : CB(cb)
	{
	}
	void Execute() { CB::_Execute(); }
	void SetResult(LoaderResult *result) { CB::m_param1 = result; }
};

class LoaderHandler
{
public:
	// ����һ������������(uint32)�ĵ�ͼ���ػص�����(LoaderResult*���㣬�����CB�Զ�����ģ����Խ�CB����һ��std::bind�������_ILoaderCallBack���ǰ��˵�һ������)
	void HandleLoaderCallback(LoaderResult* result, uint32 mapId)
	{
		if (result)
		{
			TILE_MAP_INFOS[mapId].tiles.swap(result->data);
			TILE_MAP_INFOS[mapId].ok = true;
			delete result;
		}
	}
} handler;

template<class Class, typename ParamType1>
class LoaderCallBack : public _ILoaderCallBack<MaNGOS::_Callback<Class, LoaderResult*, ParamType1> > // ����MaNGOS::_Callback�����ɵ������Ļص�����
{
	typedef MaNGOS::_Callback<Class, LoaderResult*, ParamType1> QC1;

public:
	LoaderCallBack(Class* object, typename QC1::Method method, LoaderResult* result, ParamType1 param1)
		: _ILoaderCallBack<QC1>(QC1(object, method, result, param1))
	{
	}
};

// ��������ص������Ķ��У����߳�ִ�����ݼ��ز�����Ȼ�󽫶�ȡ������ͨ��SetResult���浽һ��ILoaderCallBack���󣬲�����Ͷ�ݵ��˶��У���ɲ��в��������в�����ת��
class LoaderResultQueue : public MaNGOS::LockedQueue<ILoaderCallBack*>
{
public:
	LoaderResultQueue() {}
	void Update()
	{
		ILoaderCallBack* callback = nullptr;
		while (next(callback))
		{
			callback->Execute();
			delete callback;
		}
	}
};

// ��ͼ���ݼ�������һ��ʼ���浽�����У�Ȼ�����߳����δӶ�����ȡ������ִ��
class Loader
{
	uint32 m_mapId;
	ILoaderCallBack* m_callback;
	LoaderResultQueue* m_queue;
public:
	Loader(uint32 mapId, ILoaderCallBack* callback, LoaderResultQueue* queue): m_mapId(mapId), m_callback(callback), m_queue(queue)
	{
	}

	// ���߳���ִ�������Ķ�������Ȼ�󽫽�����浽�ص���������Ͷ�ݸ��ص�����������У��ص�����������л������߳��д��е�����ִ�����еĻص�����
	bool Execute()
	{
		boost::filesystem::path fpath((boost::format("%s/maps/%d.txt") % sWorld.GetDataPath() % m_mapId).str());
		boost::filesystem::ifstream ifs(fpath);
		if (!ifs)
		{
			sLog.outError("%d.txt open failed!", m_mapId);
			return false;
		}

		std::vector<uint8> tiles;
		for (std::string line; std::getline(ifs, line);)
		{
			if (line.front() == '#' || line.empty())
			{
				continue;
			}

			for (size_t i = 0; i < line.length(); i++)
			{
				if (line[i] == ',')
				{
					continue;
				}
				tiles.push_back(line[i]);
			}
		}
		if (!ifs.eof())
		{
			sLog.outError("%d.txt getline failed(%s)!", m_mapId, ifs.rdstate());
			return false;
		}
		// ������
		LoaderResult *result = new LoaderResult;
		result->data.swap(tiles);
		m_callback->SetResult(result);
		// �ص��������߳���ֱ��ִ�У�ֻ�������߳��д���ִ��
		m_queue->add(m_callback);

		return true;
	}

	uint32 GetID() const { return m_mapId; } // ������
};

MapManager::MapManager() {
    m_timer.SetInterval(MIN_MAP_UPDATE_DELAY);
}

MapManager::~MapManager()
{
    for (auto iter = m_maps.begin(); iter != m_maps.end(); ++iter)
	{
    	delete iter->second;
	}
}

Map* MapManager::CreateMap(uint32 id, const WorldObject* obj)
{
    MANGOS_ASSERT(obj);
    Guard _guard(*this);

    Map* pMap = nullptr;

    const MapEntry* entry = sMapStore.LookupEntry<MapEntry>(id);
    if (!entry)
	{
    	return pMap;
	}

    if (entry->Instanceable())
    {
        // ͨ��ֻ�и������ܴ������ʵ��
        MANGOS_ASSERT(obj->GetTypeId() == TYPEID_PLAYER);
        if (obj->GetTypeId() == TYPEID_PLAYER)
		{
        	pMap = CreateInstance(id, (Player*)obj);
		}
    }
    else
    {
        // ������ͨ��ͼ
		pMap = FindMap(id);
        if (pMap == nullptr)
        {
			pMap = new WorldMap(id);
            m_maps[MapID(id)] = pMap;
        }
    }

    return pMap;
}

Map* MapManager::FindMap(uint32 mapid, uint32 instanceId) const
{
    Guard guard(*this);

    auto iter = m_maps.find(MapID(mapid, instanceId));
    if (iter == m_maps.end())
	{
    	return nullptr;
	}

    if (instanceId == 0 && iter->second->Instanceable())
    {
        assert(false);
        return nullptr;
    }

    return iter->second;
}

void MapManager::Update(uint32 diff)
{
    m_timer.Update(diff);
    if (!m_timer.Passed())
	{
    	return;
	}

    // todo: ���ö��ߴ�����ͼ�ڲ����У�����ͼ��ɲ���
    for (auto iter = m_maps.begin(); iter != m_maps.end(); ++iter)
	{
    	iter->second->Update((uint32)m_timer.GetCurrent());
	}
	RemoveAllObjectsInRemoveList();

    m_timer.SetCurrent(0);
}

void MapManager::RemoveAllObjectsInRemoveList()
{
    for (auto iter = m_maps.begin(); iter != m_maps.end(); ++iter)
	{
    	iter->second->RemoveAllObjectsInRemoveList();
	}
}

void MapManager::LoadTerrains()
{
	LoaderResultQueue result;
	MaNGOS::LockedQueue<Loader*> requests;
	for (auto iter = sMapStore.getDataBegin<MapEntry>(); iter < sMapStore.getDataEnd<MapEntry>(); ++iter)
	{
		const MapEntry *proto = iter.getValue();
		MANGOS_ASSERT(proto != nullptr);
		TileMapInfo &tminfo = TILE_MAP_INFOS[proto->ID];
		tminfo.x = proto->start_x;
		tminfo.y = proto->start_y;
		tminfo.width = proto->GetWidth();
		tminfo.height = proto->GetHeight();
		tminfo.tiles.resize(tminfo.width * tminfo.height);
		// �����첽��ȡ����
		requests.add(new Loader(proto->ID, new LoaderCallBack<LoaderHandler, uint32>(&handler, &LoaderHandler::HandleLoaderCallback, (LoaderResult*)nullptr, proto->ID), &result));
	}

	std::shared_ptr<std::vector<std::thread>> workers(new std::vector<std::thread>(), [&result](std::vector<std::thread>* del) {
		uint32 thisTime = WorldTimer::getMSTime();
		for (std::thread& thr : *del)
		{
			thr.join();
		}
		delete del;

		// �첽����ص�����
		result.Update();
		sLog.outDebug("Load Terrains finish: %d ms", WorldTimer::getMSTimeDiff(thisTime, WorldTimer::getMSTime()));
	});

	// �����̴߳����첽������
	for (uint8 i = 0; i < MAX_MAP_LOADERS; ++i)
	{
		workers->push_back(std::thread([&requests]() {
			Loader* loader = nullptr;
			while (requests.next(loader))
			{
				loader->Execute();
				delete loader;
			}
		}));
	}
}

Map* MapManager::CreateInstance(uint32 id, Player* player)
{
    return nullptr;
}
