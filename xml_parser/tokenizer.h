#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include <string>

class tokenizer
{
private: // data
  typedef std::string string;

  // it and it_e are used for parsing the text
  string::const_iterator it, it_e;

  // delimiter chars are true;
  bool delimiters[256];

  // true if the current token is delimiter
  bool is_delimiter;

  bool end_reached;

  string current;


private: // methods
  bool remove_spaces ();
  void tokenize_delimiter ();
  void tokenize_quotes ();
  void tokenize_word ();
public: // methods
  tokenizer (const string &delimiters);

  // reset the tokenizer and set new text for tokenizing
  void set_text (const string &text);

  // set current_token to the next token
  // that will be returned from token () method
  // returns false if the end of the text is reached
  bool next ();

  // get the current token
  const string& token () const { return current; }

  // true if the current_token is delimiter
  bool isdelimiter () const { return this->is_delimiter; }

  // true if there is no more text to tokenize
  bool end () const { return this->end_reached; }
};
#endif /* TOKENIZER_H_ */
