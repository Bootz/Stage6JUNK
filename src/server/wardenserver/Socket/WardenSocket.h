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

#ifndef _WARDENSOCKET_H
#define _WARDENSOCKET_H

#include "Common.h"
#include "WardenDaemon.h"
#include "BufferedSocket.h"

/// Handle login commands
class WardenSocket: public BufferedSocket
{
    public:
        const static int s_BYTE_SIZE = 32;

        WardenSocket();
        ~WardenSocket();

        void OnAccept();
        void OnRead();
        void OnClose();

        bool _HandleLoadModule();
        bool _HandlePing();

    private:
        bool _connected;
};
#endif