#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
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
#include <vector>

#include "getApi.h"

std::vector<ref*> getFromApi(string doi)
{
	std::vector<ref*> reflist;
	ref* curref = NULL;

    try
    {
        std::stringstream ss;
        // send your JSON above to the parser below, but populate ss first

	    std::string refurl = "http://api.elsevier.com/content/abstract/doi/" + doi + "?apiKey=eb697e3061f267d5945a3bc3c959874a&view=REF&httpAccept=application/json";

	    curlpp::Easy c;

	    c.setOpt( new curlpp::options::Url( refurl ) );
	    c.setOpt( new curlpp::options::FollowLocation( "true" ) );
	    ss << c;

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);

        int i = 0;

        BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("abstracts-retrieval-response.references.reference")){
            boost::property_tree::ptree ptr = v.second;
            BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, ptr){
            	std::string metaurl;

            	// Create current refenence struct to fill
            	curref = new ref;

	            std::cout << v2.first.data() << ": " << v2.second.data() << std::endl;

    			if(!strcmp(v2.first.data(), "author-list") && strcmp(v2.second.data().c_str(), "null")) {
    				boost::property_tree::ptree ptr2 = v2.second;
   					BOOST_FOREACH(boost::property_tree::ptree::value_type &v3, ptr2.get_child("author")){
   						boost::property_tree::ptree ptr3 = v3.second;
   						BOOST_FOREACH(boost::property_tree::ptree::value_type &v4, ptr3){
   							std::cout << v4.first.data() << ": " << v4.second.data() << std::endl;
   						}
   					}
    			}

    			if (!strcmp(v2.first.data(), "scopus-id")) {
    				std::cout << "SECOND: " << v2.second.data() << std::endl;
    				metaurl = "http://api.elsevier.com/content/abstract/scopus_id/" + v2.second.data() + "?apiKey=eb697e3061f267d5945a3bc3c959874a&view=META&httpAccept=application/json";

    				// Clear stringstream
		        	ss.flush();

				    c.setOpt( new curlpp::options::Url( metaurl ) );
	    			c.setOpt( new curlpp::options::FollowLocation( "true" ) );
	    			ss << c;

		        	boost::property_tree::ptree rpt;
		        	boost::property_tree::read_json(ss, rpt);

		        	BOOST_FOREACH(boost::property_tree::ptree::value_type &vtec, rpt) {
		        		if (strcmp(vtec.first.data(), "service-error")) {
		        			curref->title = rpt.get<string>("abstracts-retrieval-response.coredata.dc:title");
				        	std::vector<string> strs;
				        	string barp = rpt.get<string>("abstracts-retrieval-response.coredata.prism:coverDate");
			        		boost::split(strs, barp, boost::is_any_of("-"));
			        		curref->year = strs[0];

			        		// TODO Fill these out
			        		// ref->sourceTitle = 
			            	// ref->volume = 
			            	// ref->issue =
			            	// ref->pageStart =
			            	// ref->pageEnd =
			            	// ref->doi =
			            	// ref->status =
			        	}	
			        }
    			}

	    			// if(!strcmp(v2.first.data(), "volisspag") && strcmp(v2.second.data().c_str(), "null")) {
    			// 	boost::property_tree::ptree ptr2 = v2.second;
   				// 	BOOST_FOREACH(boost::property_tree::ptree::value_type &v3, ptr2){
   				// 		boost::property_tree::ptree ptr3 = v3.second;
   				// 		BOOST_FOREACH(boost::property_tree::ptree::value_type &v4, ptr3){
   				// 			std::cout << v4.first.data() << ": " << v4.second.data() << std::endl;
       						
       // 					}
   				// 	}
    			// }
        		
        	}

        	reflist.push_back(curref);
        	std::cout << std::endl;

		}
        // return EXIT_SUCCESS;
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return reflist;
}

