IDL_NAME="OnboardATCS"

$DDS_ROOT/bin/opendds_idl ${IDL_NAME}.idl
sleep 1

tao_idl --idl-version 4 -I$DDS_ROOT -I$TAO_ROOT/orbsvcs ${IDL_NAME}.idl
sleep 1

tao_idl --idl-version 4 -I$DDS_ROOT -I$TAO_ROOT/orbsvcs ${IDL_NAME}TypeSupport.idl
sleep 1

$ACE_ROOT/bin/generate_export_file.pl ${IDL_NAME}Common > ${IDL_NAME}Common_Export.h
