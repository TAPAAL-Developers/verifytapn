#include "ArgsParser.hpp"
#include <iostream>
#include <sstream>
#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/make_shared.hpp"
#include <iomanip>
#include "boost/tokenizer.hpp"

namespace VerifyTAPN {
static const std::string KBOUND_OPTION = "k-bound";
static const std::string SEARCH_OPTION = "search-type";
static const std::string VERIFICATION_OPTION = "verification-method";
static const std::string MEMORY_OPTIMIZATION_OPTION = "memory-optimization";
static const std::string TRACE_OPTION = "trace";
static const std::string MAX_CONSTANT_OPTION = "global-max-constants";
static const std::string XML_TRACE_OPTION = "xml-trace";
static const std::string LEGACY = "legacy";
static const std::string KEEP_DEAD = "keep-dead-tokens";

std::ostream& operator<<(std::ostream& out, const Switch& flag) {
	flag.print(out);
	return out;
}

std::ostream& operator <<(std::ostream& out, const Version& version) {
	out << version.maj << "." << version.min << "." << version.build;
	return out;
}

void PrintIndentedDescription(std::ostream& out,
		const std::string& description) {
	typedef boost::tokenizer<boost::char_separator<char> > tokens;
	tokens lines(description.begin(), description.end(),
			boost::char_separator<char>("\n", "", boost::keep_empty_tokens));
	bool first = true;
	for (tokens::const_iterator it = lines.begin(); it != lines.end(); it++) {
		if (!first) {
			out << std::setw(WIDTH) << " ";
		}
		out << *it << std::endl;
		first = false;
	}
}

void Switch::print(std::ostream& out) const {
	std::stringstream s;
	s << "-" << getShortName();
	s << " [ --" << getLongName() << " ]";
	out << std::setw(WIDTH) << std::left << s.str();
	PrintIndentedDescription(out, getDescription());
}
;

void SwitchWithArg::print(std::ostream& out) const {
	std::stringstream s;
	s << "-" << getShortName();
	s << " [ --" << getLongName() << " ] ";
	s << " arg (=" << default_value << ")";
	out << std::setw(WIDTH) << std::left << s.str();
	PrintIndentedDescription(out, getDescription());
}
;

void SwitchWithStringArg::print(std::ostream& out) const {
	std::stringstream s;
	s << "-" << getShortName();
	s << " [ --" << getLongName() << " ] ";
	s << " a1,a2,.. (=" << default_value << ")";
	out << std::setw(WIDTH) << std::left << s.str();
	PrintIndentedDescription(out, getDescription());
}
;

bool Switch::handles(const std::string& flag) const {
	std::stringstream stream;
	stream << "-" << name << " ";
	if (flag.find(stream.str()) != std::string::npos)
		return true;
	if (flag.find(long_name) != std::string::npos)
		return true;
	return false;
}
;

option Switch::parse(const std::string& flag) {
	assert(handles(flag));
	handled_option = true;
	return option(getLongName(), "1");
}
;

option SwitchWithArg::parse(const std::string& flag) {
	assert(handles(flag));
	handled_option = true;
	std::string copy(flag);
	std::stringstream stream;
	stream << "-" << getShortName() << " ";
	if (flag.find(stream.str()) != std::string::npos) {
		boost::erase_all(copy, stream.str());
	} else {
		std::stringstream long_name_stream;
		long_name_stream << "--" << getLongName() << " ";
		boost::erase_all(copy, long_name_stream.str());
	}
	boost::trim(copy);
	return option(getLongName(), copy);
}
;

option SwitchWithStringArg::parse(const std::string& flag) {
	assert(handles(flag));
	handled_option = true;
	std::string copy(flag);
	std::stringstream stream;
	stream << "-" << getShortName() << " ";
	if (flag.find(stream.str()) != std::string::npos) {
		boost::erase_all(copy, stream.str());
	} else {
		std::stringstream long_name_stream;
		long_name_stream << "--" << getLongName() << " ";
		boost::erase_all(copy, long_name_stream.str());
	}
	boost::trim(copy);
	return option(getLongName(), copy);
}
;

void ArgsParser::initialize() {
    // NOTE: The Help() function only splits and indents descriptions based on newlines.
    //       Each line in the description is assumed to fit within the remaining width
    //       of the console, so keep descriptions short, or implement manual word-wrapping :).
    parsers.push_back(
            boost::make_shared<SwitchWithArg > ("k", KBOUND_OPTION,
            "Max tokens to use during exploration.", 0));
    parsers.push_back(
            boost::make_shared<SwitchWithArg > ("o", SEARCH_OPTION,
            "Specify the desired search strategy.\n - 0: Breadth-First Search\n - 1: Depth-First Search\n - 2: Random Search\n - 3: Heuristic Search",
            3));
    parsers.push_back(
            boost::make_shared<SwitchWithArg > ("m", VERIFICATION_OPTION,
            "Specify the desired verification method.\n - 0: Default (discrete)",
            0)); // TODO change to 0!
    parsers.push_back(
            boost::make_shared<SwitchWithArg > ("p", MEMORY_OPTIMIZATION_OPTION,
            "Specify the desired memory optimization.\n - 0: None \n - 1: BDD",
            1)); // TODO change to 0!
    parsers.push_back(
            boost::make_shared<SwitchWithArg > ("t", TRACE_OPTION,
            "Specify the desired trace option.\n - 0: none",
            0));
    parsers.push_back(
            boost::make_shared<Switch > ("d", KEEP_DEAD,
            "Do not discard dead tokens\n(used for boundedness checking)"));
    parsers.push_back(
            boost::make_shared<Switch > ("g", MAX_CONSTANT_OPTION,
            "Use global maximum constant for \nextrapolation (as opposed to local \nconstants)."));
    parsers.push_back(
            boost::make_shared<Switch > ("s", LEGACY,
            "Legacy option (no effect)."));

    parsers.push_back(
            boost::make_shared<Switch > ("x", XML_TRACE_OPTION,
            "Output trace in xml format for TAPAAL."));
};

void ArgsParser::printHelp() const {
	std::cout
			<< "Usage: verifydtapn -k <number> [optional arguments] model-file query-file"
			<< std::endl;
	std::cout << "Allowed Options:" << std::endl;
	std::cout << std::setw(WIDTH) << std::left << "-h [ --help ]"
			<< "Displays this help message." << std::endl;
	std::cout << std::setw(WIDTH) << std::left << "-v [ --version ]"
			<< "Displays version information." << std::endl;
	for (parser_vec::const_iterator arg = parsers.begin(); arg != parsers.end();
			arg++) {
		std::cout << **arg;
	}
}
;

void ArgsParser::printVersion() const {
	std::cout << "VerifyDTAPN " << version << std::endl;
	std::cout << "Licensed under BSD." << std::endl;
}

VerificationOptions ArgsParser::parse(int argc, char* argv[]) const {
	if (argc == 1 || std::string(argv[1]) == "-h"
			|| std::string(argv[1]) == "--help") {
		printHelp();
		exit(0);
	}

	if (std::string(argv[1]) == "-v" || std::string(argv[1]) == "--version") {
		printVersion();
		exit(0);
	}

	if (argc < 3) {
		std::cout << "too few parameters." << std::endl;
		std::cout << "Use '-h' for help on correct usage." << std::endl;
		exit(1);
	}

	std::string model_file(argv[argc - 2]);
	std::string query_file(argv[argc - 1]);

	if (boost::iends_with(query_file, ".xml")) {
		std::cout << "Missing query file." << std::endl;
		exit(1);
	}

	if (!boost::iends_with(model_file, ".xml")) {
		std::cout << "Invalid model file specified." << std::endl;
		exit(1);
	}

	if (!boost::iends_with(query_file, ".q")) {
		std::cout << "Invalid query file specified." << std::endl;
		exit(1);
	}

	std::vector<std::string> flags;
	unsigned int i = 1;
	unsigned int size = static_cast<unsigned int>(argc);
	while (i < size - 2) {
		std::string arg(argv[i]);
		if (boost::istarts_with(arg, "-")) {
			std::string arg2(argv[i + 1]);
			if (!boost::istarts_with(arg2, "-") && i + 1 < size - 2) {
				flags.push_back(arg + " " + arg2);
				i++;
			} else {
				flags.push_back(arg + " ");
			}
		}
		i++;
	}

	option_map options;
	for (std::vector<std::string>::const_iterator flag = flags.begin();
			flag != flags.end(); flag++) {
		bool handled = false;
		for (parser_vec::const_iterator it = parsers.begin();
				it != parsers.end(); it++) {
			if ((*it)->handles(*flag)) {
				options.insert((*it)->parse(*flag));
				handled = true;
			}
		}
		if (!handled) {
			std::cout << "Unknown option flag '" << *flag << "'" << std::endl;
			std::cout << "Use '-h' to see a list of valid options."
					<< std::endl;
			exit(1);
		}
	}

	// Put in default values for non-specified options
	for (parser_vec::const_iterator it = parsers.begin(); it != parsers.end();
			it++) {
		if (!(*it)->handledOption()) {
			options.insert((*it)->getDefaultOption());
		}
	}

	return createVerificationOptions(options, model_file, query_file);
}

VerificationOptions::SearchType intToSearchTypeEnum(int i) {
	switch (i) {
	case 0:
		return VerificationOptions::BREADTHFIRST;
	case 1:
		return VerificationOptions::DEPTHFIRST;
	case 2:
		return VerificationOptions::RANDOM;
	case 3:
		return VerificationOptions::COVERMOST;
	default:
		std::cout << "Unknown search strategy specified." << std::endl;
		exit(1);
	}
}

VerificationOptions::VerificationType intToVerificationTypeEnum(int i) {
	switch (i) {
	case 0:
		return VerificationOptions::DISCRETE;
	case 1:
		std::cout << "The BDD-Version only supports Discrete" << std::endl;
                exit(1);
	default:
		std::cout << "Unknown verification method specified." << std::endl;
		exit(1);
	}
}

VerificationOptions::MemoryOptimization intToMemoryOptimizationEnum(int i) {
	switch (i) {
	case 0:
		return VerificationOptions::NO_MEMORY_OPTIMIZATION;
	case 1:
		return VerificationOptions::PTRIE;
	default:
		std::cout << "Unknown memory optimization specified." << std::endl;
		exit(1);
	}
}


VerificationOptions::Trace intToEnum(int i) {
	switch (i) {
	case 0:
		return VerificationOptions::NO_TRACE;
	case 1:
                std::cout << "Traces not allowed in this BDD-version" << std::endl;
                exit(1);
	default:
		std::cout << "Unknown trace option specified." << std::endl;
		exit(1);
	}
}

unsigned int ArgsParser::tryParseInt(const option& option) const {
	unsigned int result = 0;
	try {
		result = boost::lexical_cast<unsigned int>(option.second);
	} catch (boost::bad_lexical_cast & e) {
		std::cout << "Invalid value '" << option.second << "' for option '--"
				<< option.first << "'" << std::endl;
		exit(1);
	}
	return result;
}

std::vector<std::string> ArgsParser::parseIncPlaces(
		const std::string& string) const {
	std::vector<std::string> vec;
	boost::split(vec, string, boost::is_any_of(","));
	return vec;
}

VerificationOptions ArgsParser::createVerificationOptions(const option_map& map,
		const std::string& modelFile, const std::string& queryFile) const {
	assert(map.find(KBOUND_OPTION) != map.end());
	unsigned int kbound = tryParseInt(*map.find(KBOUND_OPTION));

	assert(map.find(SEARCH_OPTION) != map.end());
	VerificationOptions::SearchType search = intToSearchTypeEnum(
			tryParseInt(*map.find(SEARCH_OPTION)));

	assert(map.find(VERIFICATION_OPTION) != map.end());
	VerificationOptions::VerificationType verification = intToVerificationTypeEnum(
			tryParseInt(*map.find(VERIFICATION_OPTION)));
        
	assert(map.find(MEMORY_OPTIMIZATION_OPTION) != map.end());
	VerificationOptions::MemoryOptimization memoptimization = intToMemoryOptimizationEnum(
			tryParseInt(*map.find(MEMORY_OPTIMIZATION_OPTION)));
        
	assert(map.find(TRACE_OPTION) != map.end());
	VerificationOptions::Trace trace = intToEnum(tryParseInt(*map.find(TRACE_OPTION)));

	assert(map.find(MAX_CONSTANT_OPTION) != map.end());
	bool max_constant = boost::lexical_cast<bool>(
			map.find(MAX_CONSTANT_OPTION)->second);

	assert(map.find(KEEP_DEAD) != map.end());
	bool keep_dead = boost::lexical_cast<bool>(map.find(KEEP_DEAD)->second);

	assert(map.find(XML_TRACE_OPTION) != map.end());
	bool xml_trace = boost::lexical_cast<bool>(
			map.find(XML_TRACE_OPTION)->second);
        
	return VerificationOptions(modelFile, queryFile, search, verification, memoptimization, kbound, trace,
			xml_trace, max_constant, keep_dead);
}
}
