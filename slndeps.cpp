/** 
 * @file slndeps.cpp
 *
 * read in a solution and generate a DOT description of the
 * dependencies. use spirit to read in the solution, then use boost
 * graph library to generate DOT code to show the graph.
 * 
 * @author D. Catanach
 * @date 2015-02-09
 */

#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_DEBUG_PRINT_SOME 100
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/uuid/uuid.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using std::vector;
using std::pair;
using std::string;
using std::istream;
using std::cerr;
using std::endl;

namespace qi = boost::spirit::qi;
//namespace phoenix = boost::phoenix;
//namespace ascii = boost::spirit::ascii;

struct project {
	string name, uuid;
	vector<string> depencencies;
};

//void astring(string s) { cerr << "\"" << s << "\"" << endl; }

// XXX parsing works! now, how do we get data out?
template <typename Iterator>
struct solutionFile : 
	qi::grammar<Iterator, string(), /*vector<project>(),*/ qi::space_type, qi::locals<char> > {
	  solutionFile() : solutionFile::base_type(Solution, "test") {
		using namespace qi;
		//using namespace phoenix;
		using boost::spirit::qi::lit;
		qi::int_parser<unsigned short, 16, 4, 4> hex_short;
		qi::int_parser<unsigned int, 16, 8, 8> hex_int;

		Solution = Header 
			>> *(Project)
			;

		Header = Header2010.alias();
		Header2010 = lit("Microsoft Visual Studio Solution File, Format Version 11.00")
			>> lit("# Visual Studio 2010");

		////8-4-4-4-12
		UUID = hex_int >> lit("-") >> hex_short >> lit("-") >>
			hex_short >> lit("-") >> hex_short >> lit("-") >>
			hex_short >> hex_int;
		Dep = lit("{") >> UUID >> lit("} = {") >> UUID >> lit("}");
		Deps = lit("ProjectSection(ProjectDependencies) = postProject") 
			>> *(Dep) 
			>> lit("EndProjectSection");
		Project = lit("Project(\"{") >> UUID >> lit("}\") = ") >> projName 
			>> lit(", ") >> projFile >> lit(", \"{") >> UUID >> lit("}\"") 
			>> !(Deps)	// match 0 or 1 times 
			>> lit("EndProject");
		projName = quoted_string.alias(); 
		projFile = quoted_string.alias(); 
		quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];

		BOOST_SPIRIT_DEBUG_NODE(Solution);
		BOOST_SPIRIT_DEBUG_NODE(Header);
		BOOST_SPIRIT_DEBUG_NODE(Header2010);
		BOOST_SPIRIT_DEBUG_NODE(quoted_string);
		BOOST_SPIRIT_DEBUG_NODE(UUID);
		BOOST_SPIRIT_DEBUG_NODE(Dep);
		BOOST_SPIRIT_DEBUG_NODE(Deps);
		BOOST_SPIRIT_DEBUG_NODE(Project);
		BOOST_SPIRIT_DEBUG_NODE(projName);
		BOOST_SPIRIT_DEBUG_NODE(projFile);
	}

	qi::rule<Iterator, string(), /*vector<project>,*/ qi::space_type, qi::locals<char> > 
		Solution, Header, Header2010, UUID, Dep, Deps, Project, projName, projFile, 
		quoted_string;

};

int main(int argc, char* argv[], char* envp[]) {
	std::ifstream in;
	in.open("DAAAC2010.sln");
	string file, str;
	while(std::getline(in, str)) file += str; 
	in.close();

	typedef std::string::const_iterator iterator_type;
	typedef solutionFile<iterator_type> solutionFile;

	//boost::spirit::ascii::space_type space;
	solutionFile sln;

	std::string::const_iterator iter = file.begin(), end = file.end();
	//vector<project> parsedSln;
	string parsedSln;
	bool r = phrase_parse(iter, end, sln, qi::space, parsedSln);
	std::cout << parsedSln << endl;
	//if(r && iter == end) {
	//} else {
	//	std::cout << "no ";
	//}
	//std::cout << "match\n";

	return 0;
}

