#ifndef IRODS_ICAT_HIGHLEVEL_ROUTINES_HPP
#define IRODS_ICAT_HIGHLEVEL_ROUTINES_HPP

#include "irods/objInfo.h"
#include "irods/ruleExecSubmit.h"
#include "irods/rcConnect.h"
#include "irods/icatStructs.hpp"
#include "irods/specificQuery.h"
#include "irods/irods_resource_manager.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <vector>
#include <map>

#include <boost/tuple/tuple.hpp>

#include "irods/irods_error.hpp"

using leaf_bundle_t = irods::resource_manager::leaf_bundle_t;

extern icatSessionStruct icss;

int chlOpen();
int chlClose();
int chlIsConnected();
int chlModDataObjMeta( rsComm_t *rsComm, dataObjInfo_t *dataObjInfo,
                       keyValPair_t *regParam );
int chlUpdateRescObjCount( const std::string& _resc, int _delta );
int chlRegDataObj( rsComm_t *rsComm, dataObjInfo_t *dataObjInfo );
int chlRegRuleExecObj( rsComm_t *rsComm,
                       ruleExecSubmitInp_t *ruleExecSubmitInp );
int chlRegReplica( rsComm_t *rsComm, dataObjInfo_t *srcDataObjInfo,
                   dataObjInfo_t *dstDataObjInfo, keyValPair_t *condInput );
int chlUnregDataObj( rsComm_t *rsComm, dataObjInfo_t *dataObjInfo,
                     keyValPair_t *condInput );
int chlRegResc( rsComm_t *rsComm, std::map<std::string, std::string>& _resc_input );
int chlAddChildResc( rsComm_t* rsComm, std::map<std::string, std::string>& _resc_input );
int chlDelResc( rsComm_t *rsComm, const std::string& _resc_name, int _dryrun = 0 ); // JMC
int chlDelChildResc( rsComm_t* rsComm, std::map<std::string, std::string>& _resc_input );
int chlRollback( rsComm_t *rsComm );
int chlCommit( rsComm_t *rsComm );
int chlDelUserRE( rsComm_t *rsComm, userInfo_t *userInfo );
int chlRegCollByAdmin( rsComm_t *rsComm, collInfo_t *collInfo );
int chlRegColl( rsComm_t *rsComm, collInfo_t *collInfo );
int chlModColl( rsComm_t *rsComm, collInfo_t *collInfo );

int chlGenQuery( genQueryInp_t genQueryInp, genQueryOut_t *result );
int chlGenQueryAccessControlSetup( const char *user, const char *zone, const char *host,
                                   int priv, int controlFlag );
int chlGenQueryTicketSetup( const char *ticket, const char *clientAddr );
int chlSpecificQuery( specificQueryInp_t specificQueryInp,
                      genQueryOut_t *genQueryOut );

int chlDelCollByAdmin( rsComm_t *rsComm, collInfo_t *collInfo );
int chlDelColl( rsComm_t *rsComm, collInfo_t *collInfo );
int chlCheckAuth( rsComm_t *rsComm, const char* scheme, const char *challenge, const char *response,
                  const char *username, int *userPrivLevel, int *clientPrivLevel );
int chlMakeTempPw( rsComm_t *rsComm, char *pwValueToHash, const char *otherUser );
int chlMakeLimitedPw( rsComm_t *rsComm, int ttl, char *pwValueToHash );
int decodePw( rsComm_t *rsComm, const char *in, char *out );
int chlModUser( rsComm_t *rsComm, const char *userName, const char *option,
                const char *newValue );
int chlModGroup( rsComm_t *rsComm, const char *groupName, const char *option,
                 const char *userName, const char *userZone );
int chlModResc( rsComm_t *rsComm, const char *rescName, const char *option,
                const char *optionValue );
int chlModRescDataPaths( rsComm_t *rsComm, const char *rescName, const char *oldPath,
                         const char *newPath, const char *userName );
int chlModRescFreeSpace( rsComm_t *rsComm, const char *rescName,
                         int updateValue );
int chlRegUserRE( rsComm_t *rsComm, userInfo_t *userInfo );
int chlAddAVUMetadata( rsComm_t *rsComm, const char *type,
                       const char *name, const char *attribute, const char *value, const char *units,
                       const KeyValPair* condInput);
int chlDeleteAVUMetadata( rsComm_t *rsComm, int option, const char *type,
                          const char *name, const char *attribute, const char *value, const char *units, int noCommit,
                          const KeyValPair* condInput);
int chlSetAVUMetadata( rsComm_t *rsComm, const char *type, // JMC - backport 4836
                       const char *name, const char *attribute, const char *newValue, const char *newUnit,
                       const KeyValPair* condInput);
int chlCopyAVUMetadata( rsComm_t *rsComm, const char *type1,  const char *type2,
                        const char *name1, const char *name2,
                        const KeyValPair* condInput);
int chlModAVUMetadata( rsComm_t *rsComm, const char *type, const char *name,
                       const char *attribute, const char *value, const char *unitsOrChange0,
                       const char *change1, const char *change2, const char *change3,
                       const KeyValPair *condInput );
int chlModAccessControl( rsComm_t *rsComm, int recursiveFlag,
                         const char* accessLevel, const char *userName, const char *zone,
                         const char* pathName );

int chlRegRuleExec( rsComm_t *rsComm, ruleExecSubmitInp_t *ruleExecSubmitInp );
int chlModRuleExec( rsComm_t *rsComm, const char *ruleExecId, keyValPair_t *regParam );
int chlDelRuleExec( rsComm_t *rsComm, const char *ruleExecId );

int chlRenameObject( rsComm_t *rsComm, rodsLong_t objId, const char *newName );
int chlMoveObject( rsComm_t *rsComm, rodsLong_t objId, rodsLong_t targetCollId );

int chlRegToken( rsComm_t *rsComm, const char *nameSpace, const char *name, const char *value,
                 const char *value2, const char *value3, const char *comment );
int chlDelToken( rsComm_t *rsComm, const char *nameSpace, const char *Name );

int chlRegZone( rsComm_t *rsComm, const char *zoneName, const char *zoneType,
                const char *zoneConnInfo, const char *zoneComment );
int chlModZone( rsComm_t *rsComm, const char *zoneName, const char *option,
                const char *optionValue );
int chlModZoneCollAcl( rsComm_t *rsComm, const char* accessLevel, const char *userName,
                       const char* pathName );
int chlDelZone( rsComm_t *rsComm, const char *zoneName );
int chlRenameLocalZone( rsComm_t *rsComm, const char *oldZoneName, const char *newZoneName );
int chlRenameColl( rsComm_t *rsComm, const char *oldName, const char *newName );

int chlRegServerLoad( rsComm_t *rsComm,
                      const char *hostName, const char *rescName,
                      const char *cpuUsed, const char *memUsed, const char *swapUsed, const char *runqLoad,
                      const char *diskSpace, const char *netInput, const char *netOutput );
int chlPurgeServerLoad( rsComm_t *rsComm, const char *secondsAgo );
int chlRegServerLoadDigest( rsComm_t *rsComm, const char *rescName, const char *loadFactor );
int chlPurgeServerLoadDigest( rsComm_t *rsComm, const char *secondsAgo );

int chlCalcUsageAndQuota( rsComm_t *rsComm );
int chlGetGridConfigurationValue(rsComm_t*   _rsComm,
                                 const char* _namespace,
                                 const char* _optionName,
                                 char*       _optionValue,
                                 std::size_t _optionValueBufferSize);
int chlSetGridConfigurationValue(rsComm_t*   _rsComm,
                                 const char* _namespace,
                                 const char* _optionName,
                                 const char* _optionValue);
int chlSetQuota( rsComm_t *rsComm, const char *type, const char *name, const char *rescName,
                 const char *limit );
int chlCheckQuota( rsComm_t *rsComm, const char *userName, const char *rescName,
                   rodsLong_t *userQuota, int *quotaStatus );
int chlDelUnusedAVUs( rsComm_t *rsComm ); // TODO Does this need a condInput too?
int chlAddSpecificQuery( rsComm_t *rsComm, const char *alias, const char *sql );
int chlDelSpecificQuery( rsComm_t *rsComm, const char *sqlOrAlias );

int chlGetLocalZone( std::string& );

int sTableInit();
int sFklink( const char *table1, const char *table2, const char *connectingSQL );
int sTable( const char *tableName, const char *tableAlias, int cycler );
int sColumn( int defineVal, const char *tableName, const char *columnName );

int chlDebug( const char *debugMode );
int chlInsRuleTable( rsComm_t *rsComm,
                     const char *baseName, const char *priorityStr, const char *ruleName,
                     const char *ruleHead, const char *ruleCondition, const char *ruleAction,
                     const char *ruleRecovery, const char *ruleIdStr, const char *myTime );
int chlVersionRuleBase( rsComm_t *rsComm,
                        const char *baseName, const char *myTime );
int chlVersionDvmBase( rsComm_t *rsComm,
                       const char *baseName, const char *myTime );
/*int chlDatabaseObjectAdmin(rsComm_t *rsComm,
  databaseObjectAdminInp_t *databaseObjectAdminInp,
  databaseObjectAdminOut_t *databaseObjectAdminOut);*/
int chlInsDvmTable( rsComm_t *rsComm,
                    const char *baseName, const char *varName, const char *action,
                    const char *var2CMap, const char *myTime );
int chlInsFnmTable( rsComm_t *rsComm,
                    const char *baseName, const char *funcName,
                    const char *func2CMap, const char *myTime );
int chlInsMsrvcTable( rsComm_t *rsComm,
                      const char *moduleName,
                      const char *msrvcName,
                      const char *msrvcSignature,
                      const char *msrvcVersion,
                      const char *msrvcHost,
                      const char *msrvcLocation,
                      const char *msrvcLanguage,
                      const char *msrvcTypeName,
                      const char *msrvcStatus,
                      const char *myTime );
int chlVersionFnmBase( rsComm_t *rsComm,
                       const char *baseName, const char *myTime );
int chlModTicket( rsComm_t *rsComm, const char *opName, const char *ticket,
                  const char *arg1, const char *arg2, const char *arg3,
                  const KeyValPair *condInput);
auto chlUpdateIrodsPamPassword(rsComm_t* _comm,
                               const char* _user_name,
                               int _ttl,
                               const char* _test_time,
                               char** _password_buffer,
                               std::size_t _password_buffer_size) -> int;

/// =-=-=-=-=-=-=-
/// @brief typedefs and prototype for query used for rebalancing operation
typedef std::vector< rodsLong_t > dist_child_result_t;

/// =-=-=-=-=-=-=-
/// @brief query which distinct data objects do not existin on a
///        given child resource which do exist on the parent
int chlGetDistinctDataObjsMissingFromChildGivenParent(
    const std::string&   _parent,
    const std::string&   _child,
    int                  _limit,
    const std::string&   _invocation_timestamp,
    dist_child_result_t& _results );

/// =-=-=-=-=-=-=-
/// @brief the the distinct data object count for a resource
int chlGetDistinctDataObjCountOnResource(
    const std::string&   _resc_name,
    long long&           _count );

int chlGetHierarchyForResc(
    const std::string&	resc_name,
    const std::string&	zone_name,
    std::string& hierarchy );

int chlCheckAndGetObjectID(
    rsComm_t*, // comm
    char*,     // type
    char*,     // name
    char* );   // access

int chlGetRcs( icatSessionStruct** );

int chlGetReplListForLeafBundles(
    rodsLong_t                  _count,
    size_t                      _child_idx,
    const std::vector<leaf_bundle_t>* _bundles,
    const std::string*          _invocation_timestamp,
    dist_child_result_t*        _results );

/// \brief High-level wrapper for database operation which calls cmlCheckDataObjId
///
/// \parblock
/// Checks to see whether the specified data_id shows up when searching for the
/// objects for which the given user has permissions to modify.
///
/// If a ticket is in use when this is called (that is, if mySessionTicket is set
/// in the database plugin), the ticket information will be checked. If the ticket
/// is expired, the bytes written with the ticket exceeds the write byte limit, or
/// the ticket is invalid for any other reason, the appropriate error code will be
/// returned. The usage count and the write file count will be updated as appropriate
/// as well.
/// \endparblock
///
/// \param[in,out] _comm iRODS comm structure
/// \param[in] _data_id Data ID for the object to check
///
/// \returns Error code based on whether authenticated user has permission\p
///          to modify the data object.
/// \retval 0 User has sufficient permission to modify the data object
/// \retval CAT_NO_ACCESS_PERMISSION User does not have sufficient permission\p
///                                  to modify the data object
///
/// \since 4.2.9
auto chl_check_permission_to_modify_data_object(RsComm& _comm, const rodsLong_t _data_id) -> int;

/// \brief High-level wrapper for database operation which calls cmlTicketUpdateWriteBytes
///
/// \parblock
/// Updates the write byte count for the ticket by the amount specified for
/// the given data object. Historically, this is usually the full size of a
/// replica for the data object being updated after some change (e.g. write).
/// \endparblock
///
/// \param[in,out] _comm iRODS comm structure
/// \param[in] _data_id Data ID for the object to check
/// \param[in] _bytes_written Number of bytes to add to write byte count
///
/// \returns Error code based on whether updating the catalog was successful
/// \retval 0 Success
///
/// \since 4.2.9
auto chl_update_ticket_write_byte_count(RsComm& _comm, const rodsLong_t _data_id, const rodsLong_t _bytes_written) -> int;

/// \brief High-level wrapper for fetching all the information about a delay rule.
///
/// Triggers policy associated with database operations.
///
/// \param[in]     _comm    The communication object.
/// \param[in]     _rule_id The ID of the delay rule.
/// \param[in,out] _info    A pointer to a vector of strings that will hold the row information.
///
/// \returns An error code representing whether the operation was successful.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \since 4.2.12
auto chl_get_delay_rule_info(RsComm& _comm, const char* _rule_id, std::vector<std::string>* _info) -> int;

/// \brief High-level wrapper for atomically updating rows in R_DATA_MAIN for all replicas of a particular data object.
//
/// \parblock
/// \p json_input must have the following JSON structure:
/// \code{.js}
/// {
///     "replicas": [
///         {
///             "before": {
///                 "data_id": <string>,
///                 "coll_id": <string>,
///                 "data_repl_num": <string>,
///                 "data_version": <string>,
///                 "data_type_name": <string>,
///                 "data_size": <string>,
///                 "data_path": <string>,
///                 "data_owner_name": <string>,
///                 "data_owner_zone": <string>,
///                 "data_is_dirty": <string>,
///                 "data_status": <string>,
///                 "data_checksum": <string>,
///                 "data_expiry_ts": <string>,
///                 "data_map_id": <string>,
///                 "data_mode": <string>,
///                 "r_comment": <string>,
///                 "create_ts": <string>,
///                 "modify_ts": <string>,
///                 "resc_id": <string>
///             },
///             "after": {
///                 "data_id": <string>,
///                 "coll_id": <string>,
///                 "data_repl_num": <string>,
///                 "data_version": <string>,
///                 "data_type_name": <string>,
///                 "data_size": <string>,
///                 "data_path": <string>,
///                 "data_owner_name": <string>,
///                 "data_owner_zone": <string>,
///                 "data_is_dirty": <string>,
///                 "data_status": <string>,
///                 "data_checksum": <string>,
///                 "data_expiry_ts": <string>,
///                 "data_map_id": <string>,
///                 "data_mode": <string>,
///                 "r_comment": <string>,
///                 "create_ts": <string>,
///                 "modify_ts": <string>,
///                 "resc_id": <string>
///             }
///         },
///         ...
///     ]
/// }
/// \endcode
/// \endparblock
///
/// \param[in] _comm iRODS comm structure
/// \param[in] _json_input String holding a JSON object with an array of replicas at key "replicas"
///
/// \returns Error code based on whether updating the catalog was successful
/// \retval 0 Success
///
/// \since 4.2.12
auto chl_data_object_finalize(RsComm& _comm, const char* _json_input) -> int;

/// \brief High-level wrapper for verifying if the native authentication credentials for a specific
///        user are correct.
///
/// Triggers policy associated with database operations.
///
/// \param[in]     _comm     The communication object.
/// \param[in]     _username The part of a fully-qualified iRODS username preceding the pound sign.
/// \param[in]     _zone     The part of a fully-qualified iRODS username following the pound sign.
/// \param[in]     _password The expected obfuscated password.
/// \param[in,out] _correct  \parblock The integer that will hold whether the credentials are correct.
///
/// If the credentials are correct, this parameter is set to 1.
/// If the credentials are incorrect, this parameter is set to 0.
/// If correctness cannot be determined, this parameter is set to -1.
/// \endparblock
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \since 4.3.1
auto chl_check_auth_credentials(RsComm& _comm,
                                const char* _username,
                                const char* _zone,
                                const char* _password,
                                int* _correct) -> int;

/// \brief High-level wrapper for executing SQL generated by GenQuery2.
///
/// Triggers policy associated with database operations.
///
/// \param[in]  _comm   The communication object.
/// \param[in]  _sql    The SQL, generated by the GenQuery2 parser, to execute.
/// \param[in]  _values The list of values to bind to the query.
/// \param[out] _output The pointer that will hold the results of the query.
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \since 4.3.2
auto chl_execute_genquery2_sql(RsComm& _comm, const char* _sql, const std::vector<std::string>* _values, char** _output)
    -> int;

/// \brief High-level wrapper for locking delay rules.
///
/// Triggers policy associated with database operations.
///
/// \param[in] _comm          The communication object.
/// \param[in] _rule_id       The ID of the delay rule to lock.
/// \param[in] _lock_host     The FQDN, hostname, or IP of the delay server.
/// \param[in] _lock_host_pid The PID of the delay server process.
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \see #rs_delay_rule_lock
///
/// \since 5.0.0
auto chl_delay_rule_lock(RsComm& _comm, const char* _rule_id, const char* _delay_rule_host, int _delay_rule_pid) -> int;

/// \brief High-level wrapper for unlocking delay rules.
///
/// Triggers policy associated with database operations.
///
/// \param[in] _comm     The communication object.
/// \param[in] _rule_ids A JSON string containing a list of delay rule IDs.
///
/// \return An integer.
/// \retval  0 On success.
/// \retval <0 On failure.
///
/// \see #rs_delay_rule_unlock
///
/// \since 5.0.0
auto chl_delay_rule_unlock(RsComm& _comm, const char* _rule_ids) -> int;

/// \brief High-level wrapper for updating the access time of one or more replicas.
///
/// Triggers policy associated with database operations.
///
/// \param[in]  _comm       The communication object.
/// \param[in]  _json_input A JSON string containing access time update information.
/// \param[out] _output     A pointer which will hold error details about the operation, if available.
///
/// \return An integer.
/// \retval >=0 The number of replicas that were affected.
/// \retval  <0 On failure.
///
/// \see #rs_update_replica_access_time
///
/// \since 5.0.0
auto chl_update_replica_access_time(RsComm& _comm, const char* _json_input, char** _output) -> int;

#endif // IRODS_ICAT_HIGHLEVEL_ROUTINES_HPP
