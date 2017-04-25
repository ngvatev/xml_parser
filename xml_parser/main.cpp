#include <iostream>
#include <fstream>
#include <string>
#include "xml_element.h"

typedef std::string string;
void test1 ()
{
  std::cout <<"test 1 is testing:"
        " xml parsing xml_element construction and output\n";
  xml_element asdf(
      "<people asd = \"DSD\">"
        "<person id = \"0\">"
          "<name>John Smith< / name>"
          "<address>USA< / address>"
        "</person>"
        "<person id = \"53\">"
          "<name>person1< / name>"
          "<address>asd< / address>"
          "</person>"
        "<person id = \"654\">"
          "<name>person2< / name>"
          "<address>meks< / address>"
        "</person>"
          "<person id = \"1\">"
          "<name>Ivan Petrov< / name>"
          "<address>Bulgaria< / address>"
        "< / person>"
      "< / people>");
  std::cout<<asdf<<"\n";
}

void test2 ()
{
  std::cout <<"test 2 is testing:"
      " xml parsing xml_element construction and output\n";
  xml_element test1 ("<people asd = \"DSD\">"
                     "<person id = \"0\">"
                     "<name>John Smith< / name>"
                     "<address>USA< / address>"
                     "</person>"
                     "<person id = \"1\">"
                     "<name>Ivan Petrov< / name>"
                     "<address>Bulgaria< / address>"
                     "< / person>"
                     "< / people>");
  std::cout<<test1<<"\n";
}


void xpath_test (xml_element &xml,
                 const string &xpath)
{
  std::vector<xml_element *> sl;
  sl = xml.xpath(xpath, sl);

  std::vector<xml_element *>::iterator it = sl.begin (), it_e = sl.end ();

  std::cout <<"xpath: "<<xpath<<"\n";
  for (it = sl.begin (), it_e = sl.end (); it != it_e; ++it)
     std::cout<<**it<<"\n";

   std::cout<<"+++++++++++++++++++++\n";
}

void xpath_tests ()
{
  xml_element xml(
      "<people asd = \"DSD\">"
        "<person id = \"0\">"
          "<name>John Smith< / name>"
          "<address>USA< / address>"
        "</person>"
        "<person id = \"53\">"
          "<name>person1< / name>"
          "<address>asd< / address>"
        "</person>"
        "<person id = \"654\">"
          "<name>person2< / name>"
          "<address>meks< / address>"
        "</person>"
        "<person id = \"1\">"
          "<name>Ivan Petrov< / name>"
          "<address>Bulgaria< / address>"
        "< / person>"
      "< / people>");

  xpath_test (xml, "person/address");
  xpath_test (xml, "person/address[0]");
  xpath_test (xml, "person[2]/address");
  xpath_test (xml, "person (@id=1)/address");
  xpath_test (xml, "person[2](@id)/address");
  xpath_test (xml, "person (address=\"USA\")");
}

int main ()
{
  //test1 ();
  xpath_tests ();

  return 0;
}
