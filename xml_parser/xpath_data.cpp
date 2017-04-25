#include "xpath_data.h"

const std::string xpath_data::delimeters_str = "/@[]=()";

bool
xpath_data::parse_xpath (const string &xpath)
{
  tokenizer xpath_tokenizer (this->delimeters_str);
  xpath_tokenizer.set_text (xpath);

  while (xpath_tokenizer.next ())
    {
      if (xpath_tokenizer.isdelimiter ())
        return false; // we are expecting xml name
      this->xml_elements_path.
        push (xml_requirements (xpath_tokenizer.token ()));

      if (xpath_tokenizer.next() == false)
        {// we have only the name of the xml element
          return true;
        }
      if (xpath_tokenizer.token ().compare ("[") == 0)
        { // we have index
          if (this->parse_square_brakets(xpath_tokenizer) == false)
            return false;
          if (xpath_tokenizer.end ())
            return true;
        }
      if (xpath_tokenizer.token ().compare ("(") == 0)
        { // we have requirements
          if (this->parse_requirements(xpath_tokenizer) == false)
            return false;
          if (xpath_tokenizer.end ())
            return true;
        }
      if (xpath_tokenizer.token ().compare ("/") != 0)
        return false; // we are xpecting end or /
    }
  return true;
}


bool
xpath_data::parse_square_brakets (tokenizer &xpath_tokenizer)
{
  // we have something like people/person[3]/address
  if (xpath_tokenizer.token ().compare ("[") != 0 ||
      xpath_tokenizer.next () == false)
    return false;

  xml_requirements &node = this->xml_elements_path.back();
  node.index = atoi (xpath_tokenizer.token ().c_str ());

  if (xpath_tokenizer.next () == false ||
      xpath_tokenizer.token ().compare ("]") != 0)
    return false;

  xpath_tokenizer.next ();
  return true;
}

bool
xpath_data::parse_attribute_requirement (tokenizer &xpath_tokenizer)
{
  // parses @id   or   @id=5   or  @id="5"
  if (xpath_tokenizer.token ().compare ("@") != 0 ||
      xpath_tokenizer.next () == false ||
      xpath_tokenizer.isdelimiter ())
    return false;

  string key, value;
  key.assign (xpath_tokenizer.token ());

  if (xpath_tokenizer.next () == false)
    return false;

  if (xpath_tokenizer.token ().compare (")") == 0 ||
      xpath_tokenizer.token ().compare ("&") == 0)
    { // we dont have value requirement so value is empty
      value.assign("");
    }
  else if (xpath_tokenizer.token ().compare ("=") == 0)
    { // we have value requirement
      if (xpath_tokenizer.next () == false ||
          xpath_tokenizer.isdelimiter ())
        return false;
      value.assign (xpath_tokenizer.token ());
      xpath_tokenizer.next ();
    }
  else
    return false;

  xml_requirements &node = this->xml_elements_path.back();
  node.attribute.
    insert(std::pair<string, string>(key, value));
  return true;
}

bool
xpath_data::parse_subxml_elemen_trequirement (tokenizer &xpath_tokenizer)
{
  // parses id=5   or  id="5"
  if (xpath_tokenizer.isdelimiter ())
    return false; // they key is not delimiter

  string key, value;
  key.assign (xpath_tokenizer.token ());

  if (xpath_tokenizer.next () == false ||
      xpath_tokenizer.token ().compare ("=") != 0 ||
      xpath_tokenizer.next () == false ||
      xpath_tokenizer.isdelimiter ())
    return false; // we are expecting = and value that is not delimiter

  value.assign (xpath_tokenizer.token ());

  xpath_tokenizer.next ();

  xml_requirements &node = this->xml_elements_path.back();
  node.subxml.
    insert(std::pair<string, string>(key, value));
  return true;
}


bool
xpath_data::parse_requirements (tokenizer &xpath_tokenizer)
{
  // requirements are in brackets ()
  if (xpath_tokenizer.token ().compare ("(") != 0 ||
      xpath_tokenizer.next () == false)
    return false;

  while (xpath_tokenizer.token ().compare(")") != 0)
    {
      if (xpath_tokenizer.token ().compare ("@") == 0)
        { // we have attribute requiremnt
          if (this->parse_attribute_requirement(xpath_tokenizer) == false)
            return false;
        }
      else
        { // we have subxml element requirement
          if (this->parse_subxml_elemen_trequirement(xpath_tokenizer) == false)
            return false;
        }
      if (xpath_tokenizer.token ().compare("&") == 0)
        {
          if (xpath_tokenizer.next() == false ||
              xpath_tokenizer.isdelimiter ())
            return false;
        }
    }
  xpath_tokenizer.next ();
  return true;
}

