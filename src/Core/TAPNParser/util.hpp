#ifndef VERIFYTAPN_UTIL_HPP_
#define VERIFYTAPN_UTIL_HPP_

#include <string>
#include <sstream>

namespace VerifyTAPN
{
	const std::string readFile(const std::string& filename);

	template<class T>
	const std::string toString(T t)
	{
		std::stringstream s;
		s << t;
		return s.str();
	};
}
#endif /* VERIFYTAPN_UTIL_HPP_ */
