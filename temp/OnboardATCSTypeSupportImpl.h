/* -*- C++ -*- */
/* Generated by /home/unichae/workspace/OpenDDS-3.14/bin/opendds_idl version 3.14 (ACE version 6.2a_p19) running on input file OnboardATCS.idl */
#ifndef OPENDDS_IDL_GENERATED_ONBOARDATCSTYPESUPPORTIMPL_H_FQO6FT
#define OPENDDS_IDL_GENERATED_ONBOARDATCSTYPESUPPORTIMPL_H_FQO6FT
#include "OnboardATCSC.h"
#include "dds/DCPS/Definitions.h"
#include "dds/DdsDcpsC.h"
#include "OnboardATCSTypeSupportC.h"
#include "dds/DCPS/Serializer.h"
#include "dds/DCPS/TypeSupportImpl.h"
#include "orbsvcs/TimeBaseC.h"


/* Begin MODULE: CORBA */


/* End MODULE: CORBA */


/* Begin MODULE: TimeBase */



/* Begin TYPEDEF: TimeT */


/* End TYPEDEF: TimeT */


/* Begin TYPEDEF: InaccuracyT */


/* End TYPEDEF: InaccuracyT */


/* Begin TYPEDEF: TdfT */


/* End TYPEDEF: TdfT */


/* Begin STRUCT: UtcT */


/* End STRUCT: UtcT */


/* Begin STRUCT: IntervalT */


/* End STRUCT: IntervalT */

/* End MODULE: TimeBase */


/* Begin MODULE: OnboardATCS */



/* Begin STRUCT: TestMessage */

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL
namespace OpenDDS { namespace DCPS {

void gen_find_size(const OnboardATCS::TestMessage& stru, size_t& size, size_t& padding);

bool operator<<(Serializer& strm, const OnboardATCS::TestMessage& stru);

bool operator>>(Serializer& strm, OnboardATCS::TestMessage& stru);

size_t gen_max_marshaled_size(const OnboardATCS::TestMessage& stru, bool align);

size_t gen_max_marshaled_size(KeyOnly<const OnboardATCS::TestMessage> stru, bool align);

void gen_find_size(KeyOnly<const OnboardATCS::TestMessage> stru, size_t& size, size_t& padding);

bool operator<<(Serializer& strm, KeyOnly<const OnboardATCS::TestMessage> stru);

bool operator>>(Serializer& strm, KeyOnly<OnboardATCS::TestMessage> stru);

template <>
struct MarshalTraits<OnboardATCS::TestMessage> {
  static bool gen_is_bounded_size() { return false; }
  static bool gen_is_bounded_key_size() { return true; }
};
}  }
OPENDDS_END_VERSIONED_NAMESPACE_DECL


namespace OnboardATCS {
/// This structure supports use of std::map with one or more keys.
struct  TestMessage_OpenDDS_KeyLessThan {
  bool operator()(const OnboardATCS::TestMessage& v1, const OnboardATCS::TestMessage& v2) const
  {
    using ::operator<; // TAO::String_Manager's operator< is in global NS
    if (v1.train_id < v2.train_id) return true;
    if (v2.train_id < v1.train_id) return false;
    return false;
  }
};
}


namespace OnboardATCS {
class TestMessageTypeSupportImpl;
}

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL
namespace OpenDDS { namespace DCPS {
template <>
struct DDSTraits<OnboardATCS::TestMessage> {
  typedef OnboardATCS::TestMessage MessageType;
  typedef OnboardATCS::TestMessageSeq MessageSequenceType;
  typedef OnboardATCS::TestMessageTypeSupport TypeSupportType;
  typedef OnboardATCS::TestMessageTypeSupportImpl TypeSupportTypeImpl;
  typedef OnboardATCS::TestMessageDataWriter DataWriterType;
  typedef OnboardATCS::TestMessageDataReader DataReaderType;
  typedef OnboardATCS::TestMessage_OpenDDS_KeyLessThan LessThanType;

  static const char* type_name () { return "OnboardATCS::TestMessage"; }
  static bool gen_has_key () { return true; }
  static size_t key_count () { return 1; }

  static size_t gen_max_marshaled_size(const MessageType& x, bool align) { return ::OpenDDS::DCPS::gen_max_marshaled_size(x, align); }
  static void gen_find_size(const MessageType& arr, size_t& size, size_t& padding) { ::OpenDDS::DCPS::gen_find_size(arr, size, padding); }

  static size_t gen_max_marshaled_size(const OpenDDS::DCPS::KeyOnly<const MessageType>& x, bool align) { return ::OpenDDS::DCPS::gen_max_marshaled_size(x, align); }
  static void gen_find_size(const OpenDDS::DCPS::KeyOnly<const MessageType>& arr, size_t& size, size_t& padding) { ::OpenDDS::DCPS::gen_find_size(arr, size, padding); }
};
}  }
OPENDDS_END_VERSIONED_NAMESPACE_DECL


namespace OnboardATCS {
class  TestMessageTypeSupportImpl
  : public virtual OpenDDS::DCPS::LocalObject<TestMessageTypeSupport>
  , public virtual OpenDDS::DCPS::TypeSupportImpl
{
public:
  typedef OpenDDS::DCPS::DDSTraits<TestMessage> TraitsType;
  typedef TestMessageTypeSupport TypeSupportType;
  typedef TestMessageTypeSupport::_var_type _var_type;
  typedef TestMessageTypeSupport::_ptr_type _ptr_type;

  TestMessageTypeSupportImpl() {}
  virtual ~TestMessageTypeSupportImpl() {}

  virtual ::DDS::DataWriter_ptr create_datawriter();
  virtual ::DDS::DataReader_ptr create_datareader();
#ifndef OPENDDS_NO_MULTI_TOPIC
  virtual ::DDS::DataReader_ptr create_multitopic_datareader();
#endif /* !OPENDDS_NO_MULTI_TOPIC */
#ifndef OPENDDS_NO_CONTENT_SUBSCRIPTION_PROFILE
  virtual const OpenDDS::DCPS::MetaStruct& getMetaStructForType();
#endif /* !OPENDDS_NO_CONTENT_SUBSCRIPTION_PROFILE */
  virtual bool has_dcps_key();
  const char* default_type_name() const;
  static TestMessageTypeSupport::_ptr_type _narrow(CORBA::Object_ptr obj);
};
}

#ifndef OPENDDS_NO_CONTENT_SUBSCRIPTION_PROFILE
OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL
namespace OpenDDS { namespace DCPS {

class MetaStruct;

template<typename T>
const MetaStruct& getMetaStruct();

template<>
const MetaStruct& getMetaStruct<OnboardATCS::TestMessage>();
bool gen_skip_over(Serializer& ser, OnboardATCS::TestMessage*);

}  }
OPENDDS_END_VERSIONED_NAMESPACE_DECL

#endif

/* End STRUCT: TestMessage */

/* End MODULE: OnboardATCS */
#endif /* OPENDDS_IDL_GENERATED_ONBOARDATCSTYPESUPPORTIMPL_H_FQO6FT */