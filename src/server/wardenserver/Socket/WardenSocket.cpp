/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Common.h"
#include "Configuration/Config.h"
#include "ByteBuffer.h"
#include "Log.h"
#include "WardenSocket.h"
#include "WardenProtocol.h"

#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_fcntl.h>
#include <ace/OS_NS_sys_stat.h>

enum eStatus
{
    STATUS_NONE         = 0,
    STATUS_CONNECTED    = 1
};

#pragma pack(push,1)

typedef struct WardenHandler
{
    eWardendOpcode cmd;
    uint32 status;
    bool (WardenSocket::*handler)(void);
}WardenHandler;

#pragma pack(pop)

const WardenHandler table[] =
{
    { MMSG_PING,                    STATUS_CONNECTED,   &WardenSocket::_HandlePing                      },
    { MMSG_LOAD_MODULE,             STATUS_CONNECTED,   &WardenSocket::_HandleLoadModule                }
};

#define WARDEN_TOTAL_COMMANDS sizeof(table)/sizeof(WardenHandler)

/// Constructor
WardenSocket::WardenSocket() : _connected(false)
{
}

/// Destructor
WardenSocket::~WardenSocket()
{
}

/// Accept the connection
void WardenSocket::OnAccept()
{
    sLog->outBasic("Accepting connection from '%s'", get_remote_address().c_str());
}

/// Connection closed
void WardenSocket::OnClose()
{
    sLog->outBasic("Terminating connection from '%s'", get_remote_address().c_str());
    _connected = false;
}

/// Read the packet from world server
void WardenSocket::OnRead()
{
    uint8 _cmd;
    while (1)
    {
        if (!_connected)
        {
            char sign[7];
            if(!recv_soft(sign, 7) || strcmp(sign, WARDEND_SIGN))
            {
                sLog->outStaticDebug("Received a connection not from Mangos '%s'.", sign);
                recv_skip(recv_len());
                return;
            }
            _connected = true;
            recv_skip(7);
            sLog->outStaticDebug("World process connected.");
        }

        if(!recv_soft((char *)&_cmd, 1))
            return;

        size_t i;

        ///- Circle through known commands and call the correct command handler
        for (i = 0; i < WARDEN_TOTAL_COMMANDS; ++i)
        {
            if ((uint8)table[i].cmd == _cmd && table[i].status == STATUS_CONNECTED && _connected)
            {
                if (!(*this.*table[i].handler)())
                    return;
                break;
            }
        }

        ///- Report unknown commands in the debug log
        if (i == WARDEN_TOTAL_COMMANDS)
        {
            sLog->outStaticDebug("[Warden] got unknown packet %u, len of received data %u", (uint32)_cmd, (uint32)recv_len());
            return;
        }
    }
}

bool WardenSocket::_HandleLoadModule()
{
    sLog->outStaticDebug("WardenSocket::_HandleLoadModule, received %u", (uint32)recv_len());
    int8 testArray[5];
    uint32 accountId;
    uint32 moduleLen;
    uint8 *module;
    uint8 sessionKey[40];
    uint8 packet[17];

    if (!recv_soft((char *)&testArray, 5)) // opcode + moduleLen
        return false;
    moduleLen = *(uint32*)(testArray + 1);
    uint32 pktSize = 1 + 4 + 4 + moduleLen + 40 + 17;
    if (recv_len() < pktSize)
    {
        sLog->outStaticDebug("Got %u bytes of data, %u bytes needed, waiting for next tick", recv_len() ,pktSize);
        return false;
    }

    recv_skip(5); // opcode + moduleLen already read

    recv((char *)&accountId, 4);
    module = (uint8*)malloc(moduleLen * sizeof(uint8));
    recv((char *)module, moduleLen);
    recv((char *)sessionKey, 40);
    recv((char *)packet, 17);

    ByteBuffer pkt;
    if (sWardend->LoadModuleAndExecute(accountId, moduleLen, module, sessionKey, packet, &pkt))
        send((char const*)pkt.contents(), pkt.size());
    else
        sLog->outBasic("There was a problem in running the sent module");
    free(module);
    return true;
}

bool WardenSocket::_HandlePing()
{
    recv_skip(1);
    ByteBuffer pkt;
    pkt << uint8(WMSG_PONG);
    //sLog->outBasic("Ping -> Pong to %s", get_remote_address().c_str());
    send((char const*)pkt.contents(), pkt.size());
    return true;
}