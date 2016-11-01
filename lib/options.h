#pragma once

#include <boost/program_options.hpp>

class options
{
public:

	options(std::string& client_id, std::string& broker);

	void add(const boost::program_options::options_description& opt)
	{
		options_.add(opt);
	}

	bool parse(int argc, char* argv[], boost::program_options::variables_map& vm);
	void help();

private:
	boost::program_options::options_description options_;

};

