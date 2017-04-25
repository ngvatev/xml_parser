#include "xml_element.h"
#include "tokenizer.h"

#include <cstdlib>
#include <iterator>
#include <cassert>
typedef std::string string;

xml_element::xml_element(std::istream &is)
{
  // make container
  string xml;
  is.seekg(0, std::ios::end);
  xml.resize(is.tellg());

  is.seekg(0, std::ios::beg);
  is.read(&xml[0], xml.size());

  xml_parser xp;

  if (xp.parse(*this, xml) == false)
    {
      assert (false && "bad xml");
      throw ("bad xml");
    }
}


xml_element::xml_element (const string &xml)
{
  xml_parser xp;
  if (xp.parse(*this, xml) == false)
    {
      assert (false && "bad xml");
      throw ("bad xml");
    }
}

xml_element::xml_element (const xml_element &xe):
    name (xe.get_name ()),
    attributes (xe.get_attributes ()),
    text (xe.get_text ())
{
  xml_elements::const_iterator it = xe.inner_xml_elements.begin (),
                               it_e = xe.inner_xml_elements.end ();
  for (; it != it_e; ++it)
    {
      inner_xml_elements.push_back (new xml_element (**it));
    }
}

xml_element& 
xml_element::operator= (const xml_element &xe)
{
  if (&xe == this)
    return *this;

  this->name.assign (xe.get_name ());
  this->attributes = xe.attributes;

  // delete inner xml elements;
  this->clear ();

  // copy inner xml elements
  xml_elements::const_iterator it = xe.inner_xml_elements.begin (),
                               it_e = xe.inner_xml_elements.end ();
  for (; it != it_e; ++it)
    {
      this->inner_xml_elements.push_back (new xml_element (**it));
    }

  this->text.assign (xe.get_text ());
  return *this;    
}

void 
xml_element::add_attribute(const string &key, const string &value)
{
  attributes.insert (std::pair<string, string> (key, value));
}

void 
xml_element::add_xml_element (const string &str, 
                              size_t pos)
{
  size_t size = this->inner_xml_elements.size ();
  assert (pos <= size);
  if (pos > size)
    {
      throw ("position is grater than the size");
    }

  xml_elements::iterator position_it;
  if (pos == size)
    position_it = this->inner_xml_elements.end ();
  else
    {
      position_it = this->inner_xml_elements.begin ();
      std::advance(position_it, pos);
    }

  this->inner_xml_elements.insert (position_it, new xml_element (str));
}

void 
xml_element::remove_attribute (const string &key)
{
  xml_attributes::iterator it = this->attributes.find (key);
  if (it == this->attributes.end ())
    return;
  attributes.erase (it);
}

void 
xml_element::remove_xml_element (size_t pos)
{
  size_t size = this->inner_xml_elements.size ();
  assert (pos < size);
  if (size <= pos)
    {
      throw ("index out of range");
    }

  xml_elements::iterator it = this->inner_xml_elements.begin ();
  std::advance(it, pos);

  delete *it;
  inner_xml_elements.erase (it);
}

const string &
xml_element::get_name () const
{
  return name;
}


const std::map<string, string>&
xml_element::get_attributes () const
{
  return this->attributes;
}

bool 
xml_element::find_attribute (const string &key) const
{
  return attributes.find (key) != attributes.end ();
}


const string& 
xml_element::get_attribute (const string &key) const
{
  std::map<string, string>::const_iterator it;
  it = attributes.find (key);
  assert (it != attributes.end () &&
          "attribute with current key does not exist");
  if (it == attributes.end ())
    {
      throw ("attribute with current key does not exist");
    }
  return it->second;
}

size_t 
xml_element::inner_xml_elements_size () const
{
  return inner_xml_elements.size ();
}

const xml_element* 
xml_element::get_inner_xml_element (size_t pos) const
{
  if (inner_xml_elements.size () <= pos)
    return NULL;

  return inner_xml_elements[pos];
}


const string& 
xml_element::get_text () const 
{
  return text;
}


xml_element::xml_elements&
xml_element::xpath (const string &xpath_str,
                    xml_elements &rl)
{
  xpath_data xpath;
  xpath.parse_xpath (xpath_str);
  std::list<xml_element*> unvisited (this->inner_xml_elements.begin (),
                                     this->inner_xml_elements.end ());
  int unvisited_size = unvisited.size ();
  while (!xpath.xml_elements_path.empty ())
    {
      int required_index = xpath.xml_elements_path.front().index,
          current_index = 0, unvisited_new_size = 0;
      bool has_index = required_index != -1 ? true : false;
      for (int i = 0; i < unvisited_size; ++i)
        { // for each check if it satisfies the path requirements
          xml_element *it = unvisited.front();
          if (it->is_result(xpath.xml_elements_path.front ()))
            { // element satisfies the requirements
              if (!has_index ||
                  (has_index && required_index == current_index))
                {
                  if (xpath.xml_elements_path.size() == 1)
                    rl.push_back(it);
                  unvisited.insert (unvisited.end (),
                                    it->inner_xml_elements.begin (),
                                    it->inner_xml_elements.end ());
                  unvisited_new_size += it->inner_xml_elements.size ();
                }
              ++current_index;
            }
          unvisited.pop_front();
        }
      unvisited_size = unvisited_new_size;
      xpath.xml_elements_path.pop ();
    }

  return rl;
}


void xml_element::clear ()
{
  xml_elements::iterator it = inner_xml_elements.begin(),
    it_e = inner_xml_elements.end();
  for (; it != it_e; ++it)
    delete  *it;
  this->name.clear ();
  this->attributes.clear ();
  this->inner_xml_elements.clear ();
  this->text.clear();
}


xml_element::~xml_element()
{
  clear ();
}


xml_element::xml_element ()
{
}


bool
xml_element::is_result (const xml_requirements &requirements)
{
  if (this->name.compare (requirements.name) != 0)
    return false;

  if (!this->verify_attributes (requirements.attribute))
    return false;

  if (!this->verify_sub_xmls (requirements.subxml))
    return false;

  return true;
}


bool
xml_element::verify_attributes (const std::map<string, string> &requirements)
{
  std::map<string, string>::const_iterator it = requirements.begin (),
                                           it_e = requirements.end ();
  for (; it != it_e; ++it)
    {
      std::map<string, string>::const_iterator pair;
      pair = this->attributes.find (it->first);
      if (this->attributes.end () == pair)
        return false;

      if (!it->second.empty () &&
          it->second.compare (pair->second) != 0)
        return false;
    }
  return true;
}


bool
xml_element::verify_sub_xmls (const std::map<string, string> &requirements)
{
  std::map<string, string>::const_iterator it = requirements.begin (),
                                           it_e = requirements.end ();
  for (; it != it_e; ++it)
    {
      xml_elements::iterator
        inner_xml_it = this->inner_xml_elements.begin (),
        inner_xml_it_e = this->inner_xml_elements.end ();
      bool found = false;
      for (; inner_xml_it != inner_xml_it_e; ++inner_xml_it)
        {
          if (it->first.compare ((*inner_xml_it)->name) != 0)
            continue;
          if (it->second.compare((*inner_xml_it)->text) != 0)
            return false;
          found = true;
          break;
        }
      if (found == false)
        return false;
    }
  return true;
}


std::ostream& operator<< (std::ostream &o, const xml_element &xml)
{
  // output name
  o << "<" << xml.name;
  // output attributes
  xml_element::xml_attributes::const_iterator map_it = xml.attributes.begin(),
                                              map_it_e = xml.attributes.end();
  for (; map_it != map_it_e; ++map_it)
    o << " " << map_it->first << "=\"" << map_it->second << "\"";
  o<<">\n";

  // output inner xml_elements
  xml_element::xml_elements::const_iterator
    list_it = xml.inner_xml_elements.begin (),
    list_it_e = xml.inner_xml_elements.end ();
  for (; list_it != list_it_e; ++list_it)
    o<<(const xml_element&)(**list_it);

  // output closing tag
  if (xml.text.length () != 0)
    o << xml.text<<"\n";
  o << "</" << xml.name << ">\n";
  return o;
}


