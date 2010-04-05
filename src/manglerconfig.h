/*
 * vim: softtabstop=4 shiftwidth=4 cindent foldmethod=marker expandtab
 *
 * $LastChangedDate$
 * $Revision$
 * $LastChangedBy$
 * $URL$
 *
 * Copyright 2009-2010 Eric Kilfoil
 *
 * This file is part of Mangler.
 *
 * Mangler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mangler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mangler.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MANGLERCONFIG_H
#define _MANGLERCONFIG_H

#include "inilib.h"

class ManglerConfig {
    public:
        iniFile         config, servers;
        //Glib::Mutex     mutex;
        ManglerConfig();
        ~ManglerConfig();
        std::string confdir() const;
        void save();
        std::vector<int> PushToTalkXKeyCodes() const;
        iniValue &operator[](const string &configVar);
        bool hasUserVolume(const string &server, const string &user) const;
        iniValue &UserVolume(const string &server, const string &user);
        bool hasUserMuted(const string &server, const string &user) const;
        iniValue &UserMuted(const string &server, const string &user);
        iniValue &ChannelPassword(const string &server, uint16_t channel);
    private:
        static const char *DefaultConfiguration;
        void ConvertOldConfig();
};

#endif
