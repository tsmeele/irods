#include "irods/irods_path_recursion.hpp"

#include "irods/rodsPath.h"
#include "irods/rodsErrorTable.h"
#include "irods/irods_exception.hpp"
#include "irods/rodsLog.h"

#include <stdlib.h>
#include <cstdlib>
#include <iomanip>

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fmt/format.h>

// Returns nothing, throws exception if file system loop is detected.
// Each path canonical path which is inserted into the map, also gets
// a second string - the user path which led us here.  This serves us
// for a descriptive error message if the same canonical path shows up
// again - with a different user path.
void
irods::check_for_filesystem_loop(boost::filesystem::path const & cpath,
                                 boost::filesystem::path const & upath,
                                 recursion_map_t &pathmap)
{
    namespace fs = boost::filesystem;

    const std::string& pathstring(cpath.string()); // Canonical path string

    recursion_map_t::iterator it(pathmap.lower_bound(pathstring));
    if (it == pathmap.end() || pathstring < it->first)
    {
        // not found - this is the first time we're seeing this canonical path
        pathmap.insert(it, make_pair(pathstring, upath.string()));     // hinted insertion

    } else {
        // File system loop discovered.
        // We have the current candidate fs:path (upath).
        // Need another fs:path for the original user path that's in the map<>
        const fs::path& origpath(it->second);

        // Either or both could be the offending symlink.
        // Both cases are displayed differently.

        std::ostringstream sstr;
        if (fs::is_symlink(upath) && fs::is_symlink(origpath))
        {
            // Both are symlinks
            sstr << "File system loop detected: Both "
                 << "\"" << upath.string() << "\""
                 << " and \"" <<  it->second << "\""
                 << " are symbolic links to canonical path \"" <<  pathstring << "\"";
        }
        else
        {
            // Only one of them is a symlink:
            // Figure out which of the two is the symlink for the message below
            const std::string sympath( (fs::is_symlink(upath)? upath.string() : it->second ));

            sstr << "File system loop detected: \"" << sympath << "\""
                 << " is a symbolic link to canonical path \"" <<  pathstring << "\"";
        }
        THROW( USER_INPUT_PATH_ERR, sstr.str().c_str() );
    }
}

// Called in places where file system loop detection is not desired/needed.
// The rodsArguments_t object is for checking for the "--ignore-symlinks" flag, and the
// character buffer is for the user filename.
// Checks for existence of path as a symlink or a directory.
// Will throw irods::exception if boost file system errors occur in the process.
bool
irods::is_path_valid_for_recursion( boost::filesystem::path const & userpath,
                                    recursion_map_t &usermap,
                                    bool dashdashlink )
{
    namespace fs = boost::filesystem;

    const fs::path normal_path = [](const fs::path& up) {
        try {
            return up.lexically_normal();
        }
        catch (const fs::filesystem_error& e) {
            THROW(USER_INPUT_PATH_ERR, fmt::format("{}\nPath = {}", e.code().message(), up.c_str()));
        }
    }(userpath);

    // Return early if the provided path is a symlink and --ignore-symlinks is being used. --ignore-symlinks means that
    // symlinks should be ignored, so the path is going to be ignored - that is, it should not be examined.
    if (dashdashlink && fs::is_symlink(normal_path)) {
        return false;
    }

    // Only call canonical once we have confirmed that symlinks are not being ignored. canonical follows symlinks and
    // makes sure that they are valid.
    const fs::path resolved = [](const fs::path& up) {
        try {
            return fs::canonical(up);
        }
        catch (const fs::filesystem_error& e) {
            THROW(USER_INPUT_PATH_ERR, fmt::format("{}\nPath = {}", e.code().message(), up.c_str()));
        }
    }(normal_path);

    const std::string& canpath = resolved.string();

    bool path_exists = [&canpath,&userpath](const fs::path& up)
        {
            try {
                return fs::exists(up);
            }
            catch(const fs::filesystem_error &_ec)
            {
                std::ostringstream sstr;
                sstr << _ec.code().message() << "\n" << "Path = " << userpath.string();
                if (canpath.size() > 0)
                {
                    sstr << "\nCanonical path = " << canpath;
                }
                THROW( USER_INPUT_PATH_ERR, sstr.str().c_str() );
            }
        }(resolved);

    if (path_exists)
    {
        if (fs::is_directory(resolved)) {
            try {
                // Adds the path to the usermap, if it's not there yet.
                // Throws an irods::exception if a loop is found (there is
                // already an instance of the path in usermap).
                check_for_filesystem_loop(resolved, userpath, usermap);
            } catch ( const irods::exception & _e ) {
                return false;
            }
        }
        // Whether it is a file or directory, it can be included.
        return true;
    }
    else
    {
        // The canonical path does not exist.  The code can
        // really never get here, because exists() would have
        // thrown an exception above. But - belt and suspenders.
        return false;
    }
}

// Called in from places where file system loop detection is not desired/needed,
// regardless of whether or not the recursion_map_t has been initialized by
// check_directories_for_loops().
//
// The rodsArguments_t object is for checking for the "--ignore-symlinks" flag, and the
// character buffer is for the user filename.
//
// Checks for existence of path as a symlink or a directory.
// Will throw irods::exception if boost file system errors occur in the process.
bool
irods::is_path_valid_for_recursion( rodsArguments_t const * const rodsArgs, const char *myPath )
{
    if (!rodsArgs || !myPath) {
        THROW(USER__NULL_INPUT_ERR, fmt::format("{}: Provided rodsArgs or myPath is null.", __func__));
    }

    recursion_map_t usermap{};
    return irods::is_path_valid_for_recursion(boost::filesystem::path{myPath}, usermap, rodsArgs->link == True);
}

// see .hpp for comment
int
irods::check_directories_for_loops( boost::filesystem::path const & dirpath,
                                    irods::recursion_map_t& pathmap,
                                    bool dashdashlink )
{
    namespace fs = boost::filesystem;
    int last_error = 0;

    try {
        // Throws an exception if the path creates a file system
        // loop, or if a boost::filesystem exception was thrown in
        // the process.  Does not care whether or not the path is valid
        // for recursion, since when it is, it will be added to the map<>
        // by is_path_valid_for_recursion(), or not added if it is not valid.
        // We only care about the exception at this point.
        irods::is_path_valid_for_recursion(dirpath, pathmap, dashdashlink);
    } catch ( const irods::exception& _e ) {
        rodsLog( LOG_ERROR, _e.client_display_what() );
        return USER_INPUT_PATH_ERR;
    }

    // This outer try/catch block will detect fs:: exceptions thrown by
    // fs::recursive_directory_iterator, which will happen if the iterator
    // comes across a directory it does not have permission to examine.
    // Whereas this causes an error that interrupts the scan, the inner
    // try/catch will simply note the error and continue the scan until
    // all symbolic links are examined.
    try {
        // Default constructor creates an end iterator
        fs::recursive_directory_iterator end_itr;
        const fs::directory_options opt =
            (dashdashlink ? fs::directory_options::none : fs::directory_options::follow_directory_symlink);
        for ( fs::recursive_directory_iterator itr( dirpath, opt ); itr != end_itr; ++itr )
        {
            fs::path p = itr->path();
            try {
                // See comment in the try{} clause above. We want to
                // continue regardless if the path is valid for recursion.
                irods::is_path_valid_for_recursion(p, pathmap, dashdashlink);
            } catch ( const irods::exception& _e ) {
                rodsLog( LOG_ERROR, _e.client_display_what() );
                last_error = USER_INPUT_PATH_ERR;
            }
        }
    } catch (const fs::filesystem_error& _fsx) {
        // This is to catch exceptions from the iterator.
        rodsLog( LOG_ERROR, _fsx.what() );

        // Cannot continue scanning - the iterator is messed up
        return USER_INPUT_PATH_ERR;
    }

    return last_error;        // Can be 0 if no errors
}

// Issue 3988: For irsync and iput mostly, scan all source physical directories
int
irods::scan_all_source_directories_for_loops(irods::recursion_map_t& pathmap,
                                             const std::vector<std::string>& dirvec,
                                             bool dashdashlink)
{
    namespace fs = boost::filesystem;

    int status = 0;
    int savedStatus = 0;

    for (auto const& srcpath: dirvec)
    {
        fs::path p(srcpath);
        if ((status = irods::check_directories_for_loops(p, pathmap, dashdashlink)) < 0 )
        {
            savedStatus = status;
        }
    }
    if (savedStatus < 0)
    {
        return savedStatus;
    }
    return status;
}

// This function does the filesystem loop and sanity check
// for both irsync and iput
int
irods::file_system_sanity_check( irods::recursion_map_t& pathmap,
                                 rodsArguments_t const * const rodsArgs,
                                 rodsPathInp_t const * const rodsPathInp )
{
    irods::scantime sctime;        // initialized to now()
    int status = 0;
    std::vector<std::string> dirvec;

    // Check if a directory scan is necessary.  Only if there is at
    // least one physical source directory targeted at a collection,
    // is the scan initiated.
    for ( int i = 0; i < rodsPathInp->numSrc; i++ )
    {
        if ( rodsPathInp->srcPath[i].objType == LOCAL_DIR_T &&
             rodsPathInp->targPath[i].objType == COLL_OBJ_T)
        {
            dirvec.push_back(const_cast<const char *>(rodsPathInp->srcPath[i].outPath));
        }
    }

    if (dirvec.size() > 0)
    {
        if ( rodsArgs->recursive != True ) {
            std::string strlist(dirvec.size() > 1? "directories " : "directory ");

            // make up the directory path list for the error message
            size_t i = 0;
            for (auto const& srcpath: dirvec)
            {
                if (i != 0)
                {
                    strlist += ", ";
                }
                strlist += "\"";
                strlist += srcpath + "\"";
                 ++i;
            }
            rodsLog( LOG_ERROR, "file_system_sanity_check: -r option must be used for %s.", strlist.c_str() );
            return USER_INPUT_OPTION_ERR;
        }

        if (rodsArgs->veryVerbose) {
            std::cout << "Running recursive pre-scan... ";
        }

        if ((status = irods::scan_all_source_directories_for_loops(pathmap,
                                                                   dirvec,
                                                                   (rodsArgs->link == True? true: false))) < 0)
        {
            std::cout << "pre-scan complete... errors found.\nAborting data transfer.\n";
            if (getenv(irods::chrono_env)) {
                std::cout << "Directory scan duration: " << sctime.get_duration_string() << " seconds\n" << std::flush;
            }
            return status;
        }
        if (rodsArgs->veryVerbose) {
            std::cout << "pre-scan complete... transferring data...\n";
        }
        if (getenv(irods::chrono_env)) {
            std::cout << "Directory scan duration: " << sctime.get_duration_string() << " seconds\n" << std::flush;
        }
    }
    return status;
}

// Issue 4006: disallow mixed files and directory sources with the
// recursive (-r) option.
int
irods::disallow_file_dir_mix_on_command_line( rodsArguments_t const * const rodsArgs,
                                                     rodsPathInp_t const * const rodsPathInp )
{
    namespace fs = boost::filesystem;

    // If the "-r" flag is not used, there's nothing to check.
    if ( rodsArgs->recursive != True )
    {
        return 0;
    }

    std::vector<std::string> filevec;

    // There cannot be any regular file sources on the command line
    for ( int i = 0; i < rodsPathInp->numSrc; i++ )
    {
        if ( rodsPathInp->srcPath[i].objType == LOCAL_FILE_T )
        {
            if (True == rodsArgs->link && fs::is_symlink(rodsPathInp->srcPath[i].outPath)) {
                continue;
            }
            filevec.push_back(const_cast<const char *>(rodsPathInp->srcPath[i].outPath));
        }
    }

    if (filevec.size() == 0)
    {
        // No regular file sources found
        return 0;
    }
    std::string strlist(filevec.size() > 1? "files " : "file ");

    // make up the directory path list for the error message
    size_t i = 0;
    for (auto const& srcpath: filevec)
    {
        if (i != 0)
        {
            strlist += ", ";
        }
        strlist += "\"";
        strlist += srcpath + "\"";
         ++i;
    }
    rodsLog( LOG_ERROR,
             "disallow_file_dir_mix_on_command_line: Cannot include regular %s on the command line with the \"-r\" flag.",
             strlist.c_str() );
    return USER_INPUT_OPTION_ERR;
}


// Class members for irods::scantime
//
irods::scantime::scantime() :
        start_(std::chrono::high_resolution_clock::now())
{ ; }

irods::scantime::~scantime()
{ ; }

std::string
irods::scantime::get_duration_string() const
{
    std::chrono::time_point<std::chrono::high_resolution_clock> endt( std::chrono::high_resolution_clock::now() );
    double elapsedTime = std::chrono::duration<double, std::milli>(endt - start_).count();

    std::ostringstream sstr;
    sstr << std::setprecision(4) << (elapsedTime / 1000.0);
    return sstr.str();
}
