
// DO NOT EDIT THIS FILE - it is machine generated -*- c++ -*-

#ifndef __gnu_CORBA_typecodes_RecordTypeCode__
#define __gnu_CORBA_typecodes_RecordTypeCode__

#pragma interface

#include <gnu/CORBA/typecodes/GeneralTypeCode.h>
extern "Java"
{
  namespace gnu
  {
    namespace CORBA
    {
        class CorbaList;
      namespace typecodes
      {
          class RecordTypeCode;
          class RecordTypeCode$Field;
      }
    }
  }
  namespace org
  {
    namespace omg
    {
      namespace CORBA
      {
          class Any;
          class StructMember;
          class TCKind;
          class TypeCode;
          class UnionMember;
          class ValueMember;
      }
    }
  }
}

class gnu::CORBA::typecodes::RecordTypeCode : public ::gnu::CORBA::typecodes::GeneralTypeCode
{

public:
  RecordTypeCode(::org::omg::CORBA::TCKind *);
  virtual void setDefaultIndex(jint);
  virtual void setDiscriminator_type(::org::omg::CORBA::TypeCode *);
  virtual ::gnu::CORBA::typecodes::RecordTypeCode$Field * getField(jint);
  virtual void add(::gnu::CORBA::typecodes::RecordTypeCode$Field *);
  virtual void add(::org::omg::CORBA::StructMember *);
  virtual void add(::org::omg::CORBA::ValueMember *);
  virtual void add(::org::omg::CORBA::UnionMember *);
  virtual jint default_index();
  virtual ::org::omg::CORBA::TypeCode * discriminator_type();
  virtual ::gnu::CORBA::typecodes::RecordTypeCode$Field * field();
  virtual jint member_count();
  virtual ::org::omg::CORBA::Any * member_label(jint);
  virtual ::java::lang::String * member_name(jint);
  virtual ::org::omg::CORBA::TypeCode * member_type(jint);
  virtual jshort member_visibility(jint);
private:
  static const jlong serialVersionUID = 1LL;
public: // actually protected
  ::gnu::CORBA::CorbaList * __attribute__((aligned(__alignof__( ::gnu::CORBA::typecodes::GeneralTypeCode)))) members;
private:
  ::org::omg::CORBA::TypeCode * discriminator_type__;
  jint default_index__;
public:
  static ::java::lang::Class class$;
};

#endif // __gnu_CORBA_typecodes_RecordTypeCode__
