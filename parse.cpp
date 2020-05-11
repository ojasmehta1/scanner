/* Complete recursive descent parser for the calculator language.
 Builds on figure 2.16.  Prints a trace of productions predicted and
 tokens matched.  Does no error recovery: prints "syntax error" and
 dies on invalid input.
 Michael L. Scott, 2008-2017.
 */
#include <iostream>
#include "scan.h"
#include <string>
using namespace  std;

const char* names[] = {"check", "read", "write", "id", "literal", "gets", "if",
"fi", "do", "od", "equal", "notequal", "smaller",
"greater", "smallerequal","greaterequal",
"add", "sub", "mul", "div", "lparen", "rparen", "eof", "eps"};

const string token_names[] = {"\"check\"", "\"read\"", "\"write\"", "id", "literal", "\"gets\"", "\"if\"",
"\"fi\"", "\"do\"", "\"od\"", "\"==\"", "\"<>\"", "\"<\"","\">\"", "\"<=\"","\">=\"","\"+\"", "\"-\"", "\"*\"", "\"/\"", 
				"\"(\"", "\")\"", "\"eof\"", "\"eps\""};

static token input_token;
static int hasError = 0;
static string image = "";
static token s_follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof};
static token r_follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof, t_fi, t_rparen};
static token e_follow[] = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof, t_fi, t_rparen, t_equal, t_notequal, t_smaller, t_greater, t_smallerequal, t_greaterequal};

string prefix(string str, string tail)
{
  if(tail == "")
    {
    return str;
    }
  for (int i = 0; i < tail.length(); ++i)
    {
    if(tail[i] == ' ')
      {
      return tail.substr(0,i)+" "+ str +" "+ tail.substr(i+1, tail.length() - i);
      }
    }
  return "prefix error";
}

int contains(token t, token set[]){
  int i = 0;
  while(set[i])
    {
    if (t == set[i++])
      {
      return 1;
      }
    }
  return 0;
}

void error ()
{
    cout << "Syntax error" << endl;
    exit (1);
}

string match (token expected)
{
    if (input_token == expected)
      {
        image = getImage();
        input_token = scan ();
      }
    else
      {
      image = getImage();
      cout << "Token " << names[input_token] << ": do you mean " << token_names[expected] << "?"<< endl;
      throw string("match");
      }
  return "";
}

string relation();
string program ();
string stmt_list ();
string stmt ();
string expr ();
string expr_tail();
string term_tail ();
string term ();
string factor_tail ();
string factor ();
string relation_op();
string add_op ();
string mul_op ();
 
string program ()
{
    try
      {
        switch (input_token)
	  {
            case t_id:
            case t_read:
            case t_write:
            case t_if:
            case t_do:
            case t_check:
            case t_eof:
	      {
                string str1 = "(program \n" ;
                str1 += "[";
                str1 += stmt_list ();
                str1 += "]\n";
                if(hasError)
		  {
		    return "";
		  }
                return str1+")\n";
	      }
            default:
	      throw string("program");
	      return "";
	  }
    }
    catch(string e)
      {
        cout << " Not expecting " << names[input_token] << " in Program"<< endl;
        return "";
      }
}

string stmt_list () {
  switch (input_token) {
      case t_id:
      case t_check:
      case t_write:
      case t_read:
      case t_if:
      case t_do:{
        string str1 = "";
        str1 += "("+stmt();
        str1 += stmt_list();
        str1 += ")\n";
        return str1;
      }
      case t_eof:
        return "\n";
      default:
	return "\n";
    }
}

string stmt ()
{
  try
    {
    switch (input_token)
      {
        case t_id:
	  {
            match (t_id);
            match (t_gets);
            string str1 = "( := (id "+ image+")" + relation();
	    str1 += ")";
	    return str1;
	  }
        case t_read:
	  {
	    match (t_read);
	    match (t_id);
            return "read (id" + image + " )\n";
	  }
        case t_write:
	  {
	    match (t_write);
	    string str1 = relation();
	    return "(write " + str1 + ")\n";
          }
        case t_if:
	  {
	    match(t_if);
	    string str1 = "(if \n";
	    str1 += relation();
	    string str2  = stmt_list();
	    match(t_fi);
            return str1 +"[\n"+ str2 + "])\n";
	  }
        case t_do:
	  {
	    match(t_do);
	    string str1 = "(do\n";
	    str1 += stmt_list();
	    match(t_od);
            return "["+ str1 + "])\n";
	  }
        case t_check:
	  {
	    match(t_check);
	    string str1 = "";
	    str1 += relation();
	    return "(check\n"+str1+")\n";
	  }
        default:
	  {
	    error();
	    return "";
	  }
      }
    }
  catch(string e)
    {
	hasError = 1;
	if(e == "match")
	{
	  cout <<" Not expecting " << token_names[input_token] << " in Statement" <<endl;
	}
      else
	{
	  cout << " Not expecting " << token_names[input_token] << " in " << e << endl;
	}
      cout << " Skipped: " << token_names[input_token] << endl;
      
      input_token = scan();
      
      while(!contains(input_token, s_follow) && input_token != t_eof)
	{
            cout << " Skipped: " << token_names[input_token] << endl;
            input_token = scan();
	}
    if(contains(input_token, s_follow))
      {
        hasError = 1;
        return "(error)\n";
      }
        return "";
      }
}

string expr ()
{
  try
    {
      string str1 = term ();
      string str2 = term_tail ();
      return prefix(str1, str2);
    }
  catch(string e)
    {
	hasError = 1;
	if(e == "match")
	  {
	    cout <<" Not expecting " << token_names[input_token] << " in expression" <<endl;
	  }
	else
	  {
	    cout << " Not expecting " << token_names[input_token] << " in " << e << endl;
	  }
	cout << " Skipped: " << token_names[input_token] << endl;
	input_token = scan();
        while(!contains(input_token, e_follow) && input_token != t_eof)
	      {
            cout << " Skipped: " << token_names[input_token] << endl;
            input_token = scan();
	      }
        if(contains(input_token, e_follow))
	  {
            hasError = 1;
            return "(error)\n";
          }
            return "";
        }
  error ();
  return "";
}

string expr_tail()
{
  switch (input_token)
    {
    case t_equal:
    case t_notequal:
    case t_smaller:
    case t_greater:
    case t_smallerequal:
    case t_greaterequal:
      {
        string str1 = relation_op();
        string str2 = expr();
        return str1+" "+str2;
      }
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
    return "";
    default:
    return "";
    }
}

string term_tail ()
{
  switch (input_token)
    {
    case t_add:
    case t_sub:
      {
        string str1 = add_op ();
        str1 += " ";
        str1 += term ();
        string str2 = term_tail ();
        return prefix(str1,str2);
      }
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
      return "";       
    default:
      return "";
    }
}

string term ()
{
  try
    {
      string str1 = factor ();
      string str2 = factor_tail ();
      return prefix(str1, str2);
    }
  catch(string e)
    {
      throw string("Term");
    }
  return "";
}

string factor_tail ()
{
  switch (input_token)
    {
    case t_mul:
    case t_div:
      {
        string str1 = mul_op ();
        string str2 = factor ();
        str1 += str2;
        str1 += factor_tail ();
        return str1 + "";
       }
    case t_add:
    case t_sub:
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
      return "";         
    default:
      return "";
    }
}

string factor ()
{
  switch (input_token)
    {
    case t_id :
      {
      match (t_id);
      string str1 = "(id"+image+")";
      return str1;
      }
    case t_literal:
      {
      match (t_literal);
      string str1 = "(lit"+image+")";
      return str1;
      }
    case t_lparen:
      {
    match (t_lparen);
    string str1 = relation ();
    match (t_rparen);
    return "("+str1+")";
      }
    default:
    throw string("Factor");
    return "";
    }
}

string relation_op()
{
  switch(input_token)
    {
    case t_equal:
      match(t_equal);
      return "== ";
    case t_notequal:
      match(t_notequal);
      return "<> ";
    case t_smaller:
      match(t_smaller);
      return "< ";
    case t_greater:
      match(t_greater);
      return "> ";
    case t_smallerequal:
      match(t_smallerequal);
      return "<= ";
    case t_greaterequal:
      match(t_greaterequal);
      return ">= ";
    default:
      throw string("Relation Operation");
      return "";
    }
}

string add_op ()
{
  switch (input_token)
    {
    case t_add:
      match (t_add);
      return "+ ";
    case t_sub:
      match (t_sub);
      return "- ";
    default:
      throw string("Arithmetic Operator");
    return "";
    }
}
 
string mul_op ()
{
  switch (input_token)
    {
    case t_mul:
      match (t_mul);
      return "* ";
    case t_div:
      match (t_div);
      return "/ ";
    default:
      throw string("Multiplication Operator");
      return "";
    }
}

string relation()
{
    try
      {
      string str2 = expr();
      string str1 = expr_tail();
      return "("+prefix(str2, str1)+ ")\n";
      }
    catch(string e)
      {
	hasError = 1;
	if(e == "match") cout <<" Not expecting " << token_names[input_token] << " in Relation" <<endl;
      else
	{
	  cout << " Not expecting " << token_names[input_token] << " in " << e << endl;
	}
      cout << " Skipped: " << token_names[input_token] << endl;
	input_token = scan();
            while(!contains(input_token, r_follow) && input_token != t_eof)
	      {
            cout << " Skipped: " << token_names[input_token] << endl;
            input_token = scan();
            cout << input_token<<endl;
	      }
        if(contains(input_token, r_follow))
	  {
            hasError = 1;
            return "(error)\n";
	  }
        return " eof";
      }
}
int main () {
    input_token = scan ();
    cout << program ();
    return 0;
}
