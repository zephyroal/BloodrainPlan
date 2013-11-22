/*
--------------------------------------------------------------------------------
This source file is part of Hydrax.
Visit ---

Copyright (C) 2008 Xavier Verguín González <xavierverguin@hotmail.com>
                                           <xavyiy@gmail.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
--------------------------------------------------------------------------------
*/

#ifndef _Hydrax_CfgFileManager_H_
#define _Hydrax_CfgFileManager_H_

#include "Prerequisites.h"

#include "Help.h"

namespace Hydrax
{
class Hydrax;

/** Config file manager.
    Class to load/save all Hydrax options from/to a config file
 */
class DllExport CfgFileManager
{
public:
    /** Constructor
        @param h Hydrax parent pointer
     */
    CfgFileManager(Hydrax* h);

    /** Destructor
     */
    ~CfgFileManager();

    /** Load hydrax cfg file
        @param File File name
            @return false if an error has been ocurred(Check the log file in this case).
     */
    const bool load(const std::string& File) const;

    /** Save current hydrax config to a file
        @param File Destination file name
            @param Path File path
            @return false if an error has been ocurred(Check the log file in this case).
     */
    const bool save(const std::string& File, const std::string& Path = "") const;

    /** <int> Get the cfg std::string
        @param Name Parameter name
        @param Value Parameter value
            @return int cfg std::string
     */
    static std::string _getCfgString(const std::string& Name, const int& Value);

    /** <float> Get the cfg std::string
        @param Name Parameter name
        @param Value Parameter value
            @return Ogre::Real cfg std::string
     */
    static std::string _getCfgString(const std::string& Name, const Ogre::Real& Value);

    /** <bool> Get the cfg std::string
        @param Name Parameter name
        @param Value Parameter value
            @return bool cfg std::string
     */
    static std::string _getCfgString(const std::string& Name, const bool& Value);

    /** <vector2> Get the cfg std::string
        @param Name Parameter name
        @param Value Parameter value
            @return Ogre::Vector2 cfg std::string
     */
    static std::string _getCfgString(const std::string& Name, const Ogre::Vector2& Value);

    /** <vector3> Get the cfg std::string
        @param Name Parameter name
        @param Value Parameter value
            @return Ogre::Vector3 cfg std::string
     */
    static std::string _getCfgString(const std::string& Name, const Ogre::Vector3& Value);

    /** <size> Get the cfg std::string
        @param Name Parameter name
        @param Value Parameter value
            @return Hydrax::Size cfg std::string
     */
    static std::string _getCfgString(const std::string& Name, const Size& Value);

    /** Get int value
        @param CfgFile Config file
        @param Name Parameter name
            @return int value
            @remarks if the parameter isn't found or the data type is not an int value, return 0 as default
     */
    static int _getIntValue(Ogre::ConfigFile& CfgFile, const std::string Name);

    /** Get float value
        @param CfgFile Config file
        @param Name Parameter name
            @return float value
            @remarks if the parameter isn't found or the data type is not a float value, return 0 as default
     */
    static Ogre::Real _getFloatValue(Ogre::ConfigFile& CfgFile, const std::string Name);

    /** Get bool value
        @param CfgFile Config file
        @param Name Parameter name
            @return bool value
            @remarks if the parameter isn't found or the data type is not a bool value, return false as default
     */
    static bool _getBoolValue(Ogre::ConfigFile& CfgFile, const std::string Name);

    /** Get vector2 value
        @param CfgFile Config file
        @param Name Parameter name
            @return vector2 value
            @remarks if the parameter isn't found or the data type is not an int value, returns (0,0) as default
     */
    static Ogre::Vector2 _getVector2Value(Ogre::ConfigFile& CfgFile, const std::string Name);

    /** Get vector3 value
        @param CfgFile Config file
        @param Name Parameter name
            @return vector3 value
            @remarks if the parameter isn't found or the data type is not an int value, returns (0,0,0) as default
     */
    static Ogre::Vector3 _getVector3Value(Ogre::ConfigFile& CfgFile, const std::string Name);

    /** Get size value
        @param CfgFile Config file
        @param Name Parameter name
            @return size value
            @remarks if the parameter isn't found or the data type is not an int value, returns (0,0) as default
     */
    static Size _getSizeValue(Ogre::ConfigFile& CfgFile, const std::string Name);

    /** Check is a std::vector<std::string> contains a specified std::string
        @param List std::string list
            @param Find std::string to find
            @return true if it's contained, false if not
     */
    static bool _isStringInList(const Ogre::StringVector& List, const std::string& Find);

private:
    /** Save a std::string in file
        @param Data Data
            @param File Destination file
            @param Path File path
            @return false if an error has ocurred
     */
    const bool _saveToFile(const std::string& Data, const std::string& File, const std::string& Path) const;

    /** Load a cfg file in an Ogre::ConfigFile
        @param File File name
            @param Result, std::pair<bool, Ogre::ConfigFile&> First: False if the file isn't in the Hydrax resource group, Second: Ogre::ConfigFile
     */
    const void _loadCfgFile(const std::string& File, std::pair<bool, Ogre::ConfigFile>& Result) const;

    /** Get components config std::string
        @return Components cfg std::string
     */
    const std::string _getComponentsCfgString() const;

    /** Load components settings
        @param CfgFile Config file
     */
    const void _loadComponentsSettings(Ogre::ConfigFile& CfgFile) const;

    /** Get rtt quality config std::string
        @return Rtt quality cfg std::string
     */
    const std::string _getRttCfgString() const;

    /** Load rtt settings
        @param CfgFile Config file
     */
    const void _loadRttSettings(Ogre::ConfigFile& CfgFile) const;

    /** Get hydrax version cfg std::string
        @return Hydrax version cfg std::string
     */
    const std::string _getVersionCfgString() const;

    /** Check hydrax version cfg file
        @param CfgFile Config file
        @return true if it's the same version, false if not.
     */
    const bool _checkVersion(Ogre::ConfigFile& CfgFile) const;

    /// Hydrax parent pointer
    Hydrax* mHydrax;
};
}

#endif
