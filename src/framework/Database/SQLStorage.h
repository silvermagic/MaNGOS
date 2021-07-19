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

#ifndef SQLSTORAGE_H
#define SQLSTORAGE_H

#include "Common.h"
#include "Log.h"
#include "Utilities/ProgressBar.h"
#include "DatabaseEnv.h"

class SQLStorageBase
{
    template<class DerivedLoader, class StorageClass> friend class SQLStorageLoaderBase;

    public:
        char const* GetTableName() const { return m_tableName; }
        char const* EntryFieldName() const { return m_entry_field; }

        FieldFormat GetDstFormat(uint32 idx) const { return (FieldFormat)m_dst_format[idx]; };
        const char* GetDstFormat() const { return m_dst_format; };
        FieldFormat GetSrcFormat(uint32 idx) const { return (FieldFormat)m_src_format[idx]; };
        const char* GetSrcFormat() const { return m_src_format; };

        uint32 GetMaxEntry() const { return m_maxEntry; };
        uint32 GetRecordCount() const { return m_recordCount; };

        template<typename T>
        class SQLSIterator
        {
            friend class SQLStorageBase;

            public:
                T const* getValue() const { return reinterpret_cast<T const*>(pointer); }

                void operator ++() { pointer += recordSize; }
                T const* operator *() const { return getValue(); }
                T const* operator ->() const { return getValue(); }
                bool operator <(const SQLSIterator& r) const { return pointer < r.pointer; }
                void operator =(const SQLSIterator& r) { pointer = r.pointer; recordSize = r.recordSize; }

            private:
                SQLSIterator(char* ptr, uint32 _recordSize) : pointer(ptr), recordSize(_recordSize) {}
                char* pointer;
                uint32 recordSize;
        };

        template<typename T>
        SQLSIterator<T> getDataBegin() const { return SQLSIterator<T>(m_data, m_recordSize); }
        template<typename T>
        SQLSIterator<T> getDataEnd() const { return SQLSIterator<T>(m_data + m_recordCount * m_recordSize, m_recordSize); }

    protected:
        SQLStorageBase();
        virtual ~SQLStorageBase() { Free(); }

        void Initialize(const char* tableName, const char* entry_field, const char* src_format, const char* dst_format);

        uint32 GetDstFieldCount() const { return m_dstFieldCount; }
        uint32 GetSrcFieldCount() const { return m_srcFieldCount; }
        uint32 GetRecordSize() const { return m_recordSize; }

        virtual void prepareToLoad(uint32 maxRecordId, uint32 recordCount, uint32 recordSize);
        virtual void JustCreatedRecord(uint32 recordId, char* record) = 0;
        virtual void Free();

    private:
        char* createRecord(uint32 recordId);

        // Information about the table
        const char* m_tableName;
        const char* m_entry_field;
        const char* m_src_format;
        const char* m_dst_format;

        // Information about the records
        uint32 m_dstFieldCount;
        uint32 m_srcFieldCount;
        uint32 m_recordCount;
        uint32 m_maxEntry;
        uint32 m_recordSize;

        // Data Storage
        char* m_data;
};

class SQLStorage : public SQLStorageBase
{
    template<class DerivedLoader, class StorageClass> friend class SQLStorageLoaderBase;

    public:
        SQLStorage(const char* fmt, const char* _entry_field, const char* sqlname);

        SQLStorage(const char* src_fmt, const char* dst_fmt, const char* _entry_field, const char* sqlname);

        ~SQLStorage() { Free(); }

        template<class T>
        T const* LookupEntry(uint32 id) const
        {
            if (id >= GetMaxEntry())
                return NULL;
            return reinterpret_cast<T const*>(m_Index[id]);
        }

        void Load();

        void EraseEntry(uint32 id);

    protected:
        void prepareToLoad(uint32 maxRecordId, uint32 recordCount, uint32 recordSize) override;
        void JustCreatedRecord(uint32 recordId, char* record) override
        {
            m_Index[recordId] = record;
        }

        void Free() override;

    private:
        // Lookup access
        char** m_Index;
};

class SQLHashStorage : public SQLStorageBase
{
    template<class DerivedLoader, class StorageClass> friend class SQLStorageLoaderBase;

    public:
        SQLHashStorage(const char* fmt, const char* _entry_field, const char* sqlname);
        SQLHashStorage(const char* src_fmt, const char* dst_fmt, const char* _entry_field, const char* sqlname);

        ~SQLHashStorage() { Free(); }

        template<class T>
        T const* LookupEntry(uint32 id) const
        {
            RecordMap::const_iterator find = m_indexMap.find(id);
            if (find != m_indexMap.end())
                return reinterpret_cast<T const*>(find->second);
            return NULL;
        }

        void Load();

        void EraseEntry(uint32 id);

    protected:
        void prepareToLoad(uint32 maxRecordId, uint32 recordCount, uint32 recordSize) override;
        void JustCreatedRecord(uint32 recordId, char* record) override
        {
            m_indexMap[recordId] = record;
        }

        void Free() override;

    private:
        typedef UNORDERED_MAP<uint32/*recordId*/, char* /*record*/> RecordMap;
        RecordMap m_indexMap;
};

class SQLMultiStorage : public SQLStorageBase
{
    template<class DerivedLoader, class StorageClass> friend class SQLStorageLoaderBase;
    template<typename T> friend class SQLMultiSIterator;
    template<typename T> friend class SQLMSIteratorBounds;

    private:
        typedef std::multimap<uint32/*recordId*/, char* /*record*/> RecordMultiMap;

    public:
        SQLMultiStorage(const char* fmt, const char* _entry_field, const char* sqlname);
        SQLMultiStorage(const char* src_fmt, const char* dst_fmt, const char* _entry_field, const char* sqlname);

        ~SQLMultiStorage() { Free(); }

        template<typename T>
        class SQLMultiSIterator
        {
            friend class SQLMultiStorage;

            public:
                T const* getValue() const { return reinterpret_cast<T const*>(citerator->second); }
                uint32 getKey() const { return citerator->first; }

                void operator ++() { ++citerator; }
                T const* operator *() const { return getValue(); }
                T const* operator ->() const { return getValue(); }
                bool operator !=(const SQLMultiSIterator& r) const { return citerator != r.citerator; }

            private:
                SQLMultiSIterator(RecordMultiMap::const_iterator _itr) : citerator(_itr) {}
                RecordMultiMap::const_iterator citerator;
        };

        template<typename T>
        class SQLMSIteratorBounds
        {
            friend class SQLMultiStorage;

            public:
                const SQLMultiSIterator<T> first;
                const SQLMultiSIterator<T> second;

            private:
                SQLMSIteratorBounds(std::pair<RecordMultiMap::const_iterator, RecordMultiMap::const_iterator> pair) : first(pair.first), second(pair.second) {}
        };

        template<typename T>
        SQLMSIteratorBounds<T> getBounds(uint32 key) const { return SQLMSIteratorBounds<T>(m_indexMultiMap.equal_range(key)); }

        void Load();

        void EraseEntry(uint32 id);

    protected:
        void prepareToLoad(uint32 maxRecordId, uint32 recordCount, uint32 recordSize) override;
        void JustCreatedRecord(uint32 recordId, char* record) override
        {
            m_indexMultiMap.insert(RecordMultiMap::value_type(recordId, record));
        }

        void Free() override;

    private:
        RecordMultiMap m_indexMultiMap;
};

template <class DerivedLoader, class StorageClass>
class SQLStorageLoaderBase
{
    public:
        void Load(StorageClass& storage, bool error_at_empty = true);

        template<class S, class D>
        void convert(uint32 field_pos, S src, D& dst);
        template<class S>
        void convert_to_str(uint32 field_pos, S src, char*& dst);
        template<class D>
        void convert_from_str(uint32 field_pos, char const* src, D& dst);
        void convert_str_to_str(uint32 field_pos, char const* src, char*& dst);
        template<class S, class D>
        void default_fill(uint32 field_pos, S src, D& dst);
        void default_fill_to_str(uint32 field_pos, char const* src, char*& dst);

        // trap, no body
        template<class D>
        void convert_from_str(uint32 field_pos, char* src, D& dst);
        void convert_str_to_str(uint32 field_pos, char* src, char*& dst);

    private:
        template<class V>
        void storeValue(V value, StorageClass& store, char* record, uint32 field_pos, uint32& offset);
        void storeValue(char const* value, StorageClass& store, char* record, uint32 field_pos, uint32& offset);

        // trap, no body
        void storeValue(char* value, StorageClass& store, char* record, uint32 field_pos, uint32& offset);
};

class SQLStorageLoader : public SQLStorageLoaderBase<SQLStorageLoader, SQLStorage>
{
};

class SQLHashStorageLoader : public SQLStorageLoaderBase<SQLHashStorageLoader, SQLHashStorage>
{
};

class SQLMultiStorageLoader : public SQLStorageLoaderBase<SQLMultiStorageLoader, SQLMultiStorage>
{
};


template<class DerivedLoader, class StorageClass>
template<class S, class D>                                  // S source-type, D destination-type
void SQLStorageLoaderBase<DerivedLoader, StorageClass>::convert(uint32 /*field_pos*/, S src, D& dst)
{
    dst = D(src);
}

template<class DerivedLoader, class StorageClass>
void SQLStorageLoaderBase<DerivedLoader, StorageClass>::convert_str_to_str(uint32 /*field_pos*/, char const* src, char*& dst)
{
    if (!src)
    {
        dst = new char[1];
        *dst = 0;
    }
    else
    {
        uint32 l = strlen(src) + 1;
        dst = new char[l];
        memcpy(dst, src, l);
    }
}

template<class DerivedLoader, class StorageClass>
template<class S>                                           // S source-type
void SQLStorageLoaderBase<DerivedLoader, StorageClass>::convert_to_str(uint32 /*field_pos*/, S /*src*/, char*& dst)
{
    dst = new char[1];
    *dst = 0;
}

template<class DerivedLoader, class StorageClass>
template<class D>                                           // D destination-type
void SQLStorageLoaderBase<DerivedLoader, StorageClass>::convert_from_str(uint32 /*field_pos*/, char const* /*src*/, D& dst)
{
    dst = 0;
}

template<class DerivedLoader, class StorageClass>
template<class S, class D>                                  // S source-type, D destination-type
void SQLStorageLoaderBase<DerivedLoader, StorageClass>::default_fill(uint32 /*field_pos*/, S src, D& dst)
{
    dst = D(src);
}

template<class DerivedLoader, class StorageClass>
void SQLStorageLoaderBase<DerivedLoader, StorageClass>::default_fill_to_str(uint32 /*field_pos*/, char const* /*src*/, char*& dst)
{
    dst = new char[1];
    *dst = 0;
}

template<class DerivedLoader, class StorageClass>
template<class V>                                           // V value-type
void SQLStorageLoaderBase<DerivedLoader, StorageClass>::storeValue(V value, StorageClass& store, char* p, uint32 x, uint32& offset)
{
    DerivedLoader* subclass = (static_cast<DerivedLoader*>(this));
    switch (store.GetDstFormat(x))
    {
        case FT_LOGIC:
            subclass->convert(x, value, *((bool*)(&p[offset])));
            offset += sizeof(bool);
            break;
        case FT_BYTE:
            subclass->convert(x, value, *((char*)(&p[offset])));
            offset += sizeof(char);
            break;
        case FT_INT:
            subclass->convert(x, value, *((uint32*)(&p[offset])));
            offset += sizeof(uint32);
            break;
        case FT_FLOAT:
            subclass->convert(x, value, *((float*)(&p[offset])));
            offset += sizeof(float);
            break;
        case FT_STRING:
            subclass->convert_to_str(x, value, *((char**)(&p[offset])));
            offset += sizeof(char*);
            break;
        case FT_NA:
            subclass->default_fill(x, value, *((uint32*)(&p[offset])));
            offset += sizeof(uint32);
            break;
        case FT_NA_BYTE:
            subclass->default_fill(x, value, *((char*)(&p[offset])));
            offset += sizeof(char);
            break;
        case FT_NA_FLOAT:
            subclass->default_fill(x, value, *((float*)(&p[offset])));
            offset += sizeof(float);
            break;
        case FT_IND:
        case FT_SORT:
            assert(false && "SQL storage not have sort field types");
            break;
        default:
            assert(false && "unknown format character");
            break;
    }
}

template<class DerivedLoader, class StorageClass>
void SQLStorageLoaderBase<DerivedLoader, StorageClass>::storeValue(char const* value, StorageClass& store, char* p, uint32 x, uint32& offset)
{
    DerivedLoader* subclass = (static_cast<DerivedLoader*>(this));
    switch (store.GetDstFormat(x))
    {
        case FT_LOGIC:
            subclass->convert_from_str(x, value, *((bool*)(&p[offset])));
            offset += sizeof(bool);
            break;
        case FT_BYTE:
            subclass->convert_from_str(x, value, *((char*)(&p[offset])));
            offset += sizeof(char);
            break;
        case FT_INT:
            subclass->convert_from_str(x, value, *((uint32*)(&p[offset])));
            offset += sizeof(uint32);
            break;
        case FT_FLOAT:
            subclass->convert_from_str(x, value, *((float*)(&p[offset])));
            offset += sizeof(float);
            break;
        case FT_STRING:
            subclass->convert_str_to_str(x, value, *((char**)(&p[offset])));
            offset += sizeof(char*);
            break;
        case FT_NA_POINTER:
            subclass->default_fill_to_str(x, value, *((char**)(&p[offset])));
            offset += sizeof(char*);
            break;
        case FT_IND:
        case FT_SORT:
            assert(false && "SQL storage not have sort field types");
            break;
        default:
            assert(false && "unknown format character");
            break;
    }
}

template<class DerivedLoader, class StorageClass>
void SQLStorageLoaderBase<DerivedLoader, StorageClass>::Load(StorageClass& store, bool error_at_empty /*= true*/)
{
    Field* fields = NULL;
    QueryResult* result  = WorldDatabase.PQuery("SELECT MAX(%s) FROM %s", store.EntryFieldName(), store.GetTableName());
    if (!result)
    {
        sLog.outError("Error loading %s table (not exist?)\n", store.GetTableName());
        Log::WaitBeforeContinueIfNeed();
        exit(1);                                            // Stop server at loading non exited table or not accessable table
    }

    uint32 maxRecordId = (*result)[0].GetUInt32() + 1;
    uint32 recordCount = 0;
    uint32 recordsize = 0;
    delete result;

    result = WorldDatabase.PQuery("SELECT COUNT(*) FROM %s", store.GetTableName());
    if (result)
    {
        fields = result->Fetch();
        recordCount = fields[0].GetUInt32();
        delete result;
    }

    result = WorldDatabase.PQuery("SELECT * FROM %s", store.GetTableName());

    if (!result)
    {
        if (error_at_empty)
            sLog.outError("%s table is empty!\n", store.GetTableName());
        else
            sLog.outString("%s table is empty!\n", store.GetTableName());

        recordCount = 0;
        return;
    }

    if (store.GetSrcFieldCount() != result->GetFieldCount())
    {
        recordCount = 0;
        sLog.outError("Error in %s table, probably sql file format was updated (there should be %d fields in sql).\n", store.GetTableName(), store.GetSrcFieldCount());
        delete result;
        Log::WaitBeforeContinueIfNeed();
        exit(1);                                            // Stop server at loading broken or non-compatible table.
    }

    // get struct size
    uint32 offset = 0;
    for (uint32 x = 0; x < store.GetDstFieldCount(); ++x)
    {
        switch (store.GetDstFormat(x))
        {
            case FT_LOGIC:
                recordsize += sizeof(bool);   break;
            case FT_BYTE:
                recordsize += sizeof(char);   break;
            case FT_INT:
                recordsize += sizeof(uint32); break;
            case FT_FLOAT:
                recordsize += sizeof(float);  break;
            case FT_STRING:
                recordsize += sizeof(char*);  break;
            case FT_NA:
                recordsize += sizeof(uint32); break;
            case FT_NA_BYTE:
                recordsize += sizeof(char);   break;
            case FT_NA_FLOAT:
                recordsize += sizeof(float);  break;
            case FT_NA_POINTER:
                recordsize += sizeof(char*);  break;
            case FT_IND:
            case FT_SORT:
                assert(false && "SQL storage not have sort field types");
                break;
            default:
                assert(false && "unknown format character");
                break;
        }
    }

    // Prepare data storage and lookup storage
    store.prepareToLoad(maxRecordId, recordCount, recordsize);

    BarGoLink bar(recordCount);
    do
    {
        fields = result->Fetch();
        bar.step();

        char* record = store.createRecord(fields[0].GetUInt32());
        offset = 0;

        // dependend on dest-size
        // iterate two indexes: x over dest, y over source
        //                      y++ If and only If x != FT_NA*
        //                      x++ If and only If a value is stored
        for (uint32 x = 0, y = 0; x < store.GetDstFieldCount();)
        {
            switch (store.GetDstFormat(x))
            {
                // For default fill continue and do not increase y
                case FT_NA:         storeValue((uint32)0, store, record, x, offset);         ++x; continue;
                case FT_NA_BYTE:    storeValue((char)0, store, record, x, offset);           ++x; continue;
                case FT_NA_FLOAT:   storeValue((float)0.0f, store, record, x, offset);       ++x; continue;
                case FT_NA_POINTER: storeValue((char const*)NULL, store, record, x, offset); ++x; continue;
            }

            // It is required that the input has at least as many columns set as the output requires
            if (y >= store.GetSrcFieldCount())
                assert(false && "SQL storage has too few columns!");

            switch (store.GetSrcFormat(y))
            {
                case FT_LOGIC:  storeValue((bool)(fields[y].GetUInt32() > 0), store, record, x, offset);  ++x; break;
                case FT_BYTE:   storeValue((char)fields[y].GetUInt8(), store, record, x, offset);         ++x; break;
                case FT_INT:    storeValue((uint32)fields[y].GetUInt32(), store, record, x, offset);      ++x; break;
                case FT_FLOAT:  storeValue((float)fields[y].GetFloat(), store, record, x, offset);        ++x; break;
                case FT_STRING: storeValue((char const*)fields[y].GetString(), store, record, x, offset); ++x; break;
                case FT_NA:
                case FT_NA_BYTE:
                case FT_NA_FLOAT:
                    // Do Not increase x
                    break;
                case FT_IND:
                case FT_SORT:
                case FT_NA_POINTER:
                    assert(false && "SQL storage not have sort or pointer field types");
                    break;
                default:
                    assert(false && "unknown format character");
            }
            ++y;
        }
    }
    while (result->NextRow());

    delete result;
}

#endif
