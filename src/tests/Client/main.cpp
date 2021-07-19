#include <boost/asio.hpp>
#include "Config/Config.h"
#include "Log.h"
#include "Util.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "SystemConfig.h"

using boost::asio::ip::tcp;

int main(int argc, char *argv[])
{
	if (!sConfig.SetSource("mangosd.conf.dist"))
	{
		sLog.outError("Could not find configuration file mangosd.conf.dist.");
		return 1;
	}

	try
	{
		boost::asio::io_context ioContext;
		tcp::socket socket(ioContext);
		tcp::endpoint ep(boost::asio::ip::make_address("127.0.0.1"), sConfig.GetIntDefault("WorldServerPort", DEFAULT_WORLDSERVER_PORT));
		socket.connect(ep);

		{
			WorldPacket packet(C_OPCODE_KEEPALIVE, 0);
			packet << uint8(0x0);
			packet << uint8(0x0);
			packet << uint8(0x0);

			uint16 size = sizeof(ClientPktHeader) + sizeof(OpcodeType) + packet.size();
			ClientPktHeader header;
			header.Size = size;
			EndianConvert(header.Size);
			MessageBuffer buffer(size);
			buffer.Write(&header, sizeof(ClientPktHeader));
			OpcodeType opcode = packet.GetOpcode();
			buffer.Write(&opcode, sizeof(OpcodeType));
			buffer.Write(packet.contents(), packet.size());
			boost::asio::async_write(socket,
				boost::asio::buffer(buffer.GetReadPointer(), buffer.GetActiveSize()),
				[&buffer](boost::system::error_code ec, std::size_t size) {
					if (!ec) {
						buffer.ReadCompleted(size);
					} else {
						sLog.outError("send error");
					}
				});
		}

		{
			const uint8 data[] = {0x33, 0x00, 0xa8, 0x03, 0x00, 0x00, 0x00, 0xd4, 0xb0, 0x01, 0x00};
			WorldPacket packet(C_OPCODE_CLIENTVERSION, 0);
			packet.append(data, sizeof(data));
			uint16 size = sizeof(ClientPktHeader) + sizeof(OpcodeType) + packet.size();
			ClientPktHeader header;
			header.Size = size;
			EndianConvert(header.Size);
			MessageBuffer buffer(size);
			buffer.Write(&header, sizeof(ClientPktHeader));
			OpcodeType opcode = packet.GetOpcode();
			buffer.Write(&opcode, sizeof(OpcodeType));
			buffer.Write(packet.contents(), packet.size());
			boost::asio::async_write(socket,
				boost::asio::buffer(buffer.GetReadPointer(), buffer.GetActiveSize()),
				[&buffer, &socket](boost::system::error_code ec, std::size_t size) {
					if (!ec) {
						buffer.ReadCompleted(size);
						buffer.Reset();
						buffer.Resize(sizeof(ServerPktHeader) + sizeof(OpcodeType) + sizeof(uint32));
						boost::asio::async_read(socket,
							boost::asio::buffer(buffer.GetWritePointer(), buffer.GetRemainingSpace()),
							[&buffer](boost::system::error_code ec, std::size_t size) {
								if (!ec) {
									buffer.WriteCompleted(size);
									ServerPktHeader header;
									header.Size = ((ServerPktHeader*)buffer.GetReadPointer())->Size;
									buffer.ReadCompleted(sizeof(ServerPktHeader));
									EndianConvert(header.Size);
									OpcodeType opcode;
									opcode = *(OpcodeType*)buffer.GetReadPointer();
									sLog.outDebug("Opcode: %d Size: %d\n%s", header.Size, opcode, BytesToString(buffer.GetReadPointer(), buffer.GetActiveSize()).c_str());
								} else {
									sLog.outError("send error");
								}
							});
					} else {
						sLog.outError("send error");
					}
				});
		}

		ioContext.run();
	}
	catch (const boost::system::system_error &err) {
		sLog.outError("%s", err.what());
	}

	return 0;
}