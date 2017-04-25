#include "tokenizer.h"

tokenizer::tokenizer (const string &delimiters) :
                      delimiters (),
                      is_delimiter (false),
                      end_reached (true)
{
  string::const_iterator delimiters_it = delimiters.begin (),
                         delimiters_it_e = delimiters.end ();

  // set delimiters ex. delimiters['>'] = true;
  for (; delimiters_it != delimiters_it_e; ++delimiters_it)
    this->delimiters[static_cast<int>(*delimiters_it)] = true;
}

void
tokenizer::set_text (const string &text)
{
  this->it = text.begin ();
  this->it_e = text.end ();
  this->end_reached = it == it_e ? true : false;
  this->current.clear();
}

bool tokenizer::next ()
{
  // eat spaces
  if (this->remove_spaces () == false)
    {
      return false;
    }

  this->is_delimiter = false;
  if (delimiters[static_cast<int> (*it)])
    { // we have delimiter
      this->tokenize_delimiter ();
    }
  else if (*it == '"')
    { // we have quotes where special chars are ignored
      this->tokenize_quotes ();
      if (this->end_reached)
        { // if we have end we dont have a closing quote
          return false;
        }
    }
  else
    { // we have a word
      this->tokenize_word ();
    }

  return true;
}


bool
tokenizer::remove_spaces ()
{
  // remove spaces
  while (it != it_e && isspace(*it))
    ++it;

  // check for end
  if (it == it_e)
    {
      this->end_reached = true;
      return false;
    }

  return true;
}

void
tokenizer::tokenize_delimiter ()
{
  string::const_iterator token_begin = it;
  this->is_delimiter = true;
  ++it;
  current.assign(token_begin, it);
}

void
tokenizer::tokenize_quotes ()
{
  string::const_iterator token_begin = it;

  // we dont need the first quote
  token_begin = ++it;

  // ignore everything until closing quote
  while (it != it_e && *it != '"')
    {
      ++it;
    }

  // end of text with no closing quote;
  if (it == it_e)
    {
      this->end_reached = true;
    }

  // we dont need the closing quote quote;
  current.assign(token_begin, it);
  ++it; // we just skip it
}

void
tokenizer::tokenize_word ()
{
  string::const_iterator token_begin = it;

  // the end of the token will be until:
  // the end of text or a space or a delimiter
  while (it != it_e &&
         !isspace(*it) &&
         !delimiters[static_cast<int> (*it)])
    {
      ++it;
    }

  if (it == it_e)
    {
      this->end_reached = true;
    }

  current.assign(token_begin, it);
}



