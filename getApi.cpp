#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>


int main()
{
    try
    {
        std::stringstream ss;
        // send your JSON above to the parser below, but populate ss first

	    std::string base = "http://api.elsevier.com/content/abstract/doi/10.1049/iet-sen.2009.0084?apiKey=eb697e3061f267d5945a3bc3c959874a&view=REF&httpAccept=application/json";
	    std::string function = "access/list";
	    std::string username = "testuser";
	    std::string password = "testpass";

	    curlpp::Easy c;

	    c.setOpt( new curlpp::options::Url( base ) );
	    c.setOpt( new curlpp::options::FollowLocation( "true" ) );
	    ss << c;

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);

        int i = 0;

        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("abstracts-retrieval-response.references.reference")){

            // std::cout << v.first.data() << std::endl;

            // std::cout << v.second.data() << std::endl;

            boost::property_tree::ptree ptr = v.second;
            BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, ptr){
    	            std::cout << v2.first.data() << ": " << v2.second.data() << std::endl;

        			if(!strcmp(v2.first.data(), "author-list")) {
        				boost::property_tree::ptree ptr2 = v2.second;
           					BOOST_FOREACH(boost::property_tree::ptree::value_type &v3, ptr2.get_child("author")){
           						boost::property_tree::ptree ptr3 = v3.second;
           						BOOST_FOREACH(boost::property_tree::ptree::value_type &v4, ptr3){
           							std::cout << v4.first.data() << ": " << v4.second.data() << std::endl;
           						}
           					}

        			}
        				
        	}

        	std::cout << std::endl;

		}
        return EXIT_SUCCESS;
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

