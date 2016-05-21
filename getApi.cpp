#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/optional/optional.hpp>
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

void* getFromApi(void* param)
{
    string &doi = *static_cast<string*>(param);

	std::vector<ref*> reflist;
	std::vector<corRef*> corRefList;
	ref* curref = NULL;
	corRef* corCurRef = NULL;

    try
    {
        std::stringstream ss;
	    std::string refurl = "http://api.elsevier.com/content/abstract/doi/" + doi + "?apiKey=eb697e3061f267d5945a3bc3c959874a&view=FULL&httpAccept=application/json";

	    curlpp::Easy c;
	    c.setOpt( new curlpp::options::Url( refurl ) );
	    c.setOpt( new curlpp::options::FollowLocation( "true" ) );
	    ss << c;

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);

        boost::optional< boost::property_tree::ptree& > checkptr = pt.get_child_optional("abstracts-retrieval-response.coredata");

        if(checkptr) {
            BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("abstracts-retrieval-response.item.bibrecord.tail.bibliography.reference")) {
                curref = new ref;
                boost::property_tree::ptree ptr = v.second;

                checkptr = ptr.get_child_optional("ref-info.ref-title.ref-titletext");
                if(checkptr) {
                    curref->title = ptr.get<string>("ref-info.ref-title.ref-titletext");
                }

                checkptr = ptr.get_child_optional("ref-info.ref-publicationyear.@first");
                if(checkptr) {
                    curref->year = ptr.get<string>("ref-info.ref-publicationyear.@first");
                }

                checkptr = ptr.get_child_optional("ref-info.ref-volisspag.voliss.@volume");
                if(checkptr) {
                    curref->volume = ptr.get<string>("ref-info.ref-volisspag.voliss.@volume");
                }

                checkptr = ptr.get_child_optional("ref-info.ref-volisspag.voliss.@issue");
                if(checkptr) {
                    curref->issue = ptr.get<string>("ref-info.ref-volisspag.voliss.@issue");
                }

                checkptr = ptr.get_child_optional("ref-info.ref-volisspag.pagerange.@first");
                if(checkptr) {
                    curref->pageStart = ptr.get<string>("ref-info.ref-volisspag.pagerange.@first");
                }

                checkptr = ptr.get_child_optional("ref-info.ref-volisspag.pagerange.@last");
                if(checkptr) {
                    curref->pageEnd = ptr.get<string>("ref-info.ref-volisspag.pagerange.@last");
                } 

                curref->status = "unsure";

                reflist.push_back(curref);
            }
        }

        refurl = "http://api.elsevier.com/content/abstract/doi/" + doi + "?apiKey=eb697e3061f267d5945a3bc3c959874a&view=REF&httpAccept=application/json";

        c.setOpt( new curlpp::options::Url( refurl ) );
        c.setOpt( new curlpp::options::FollowLocation( "true" ) );
        ss << c;

        boost::property_tree::read_json(ss, pt);

        checkptr = pt.get_child_optional("abstracts-retrieval-response.references.reference");
        int i = 0;

        vector<prepComp> idList;
        prepComp id;
        //fill array before such that if there are any missing it is in the array but will not 
        //query scopus, because it cannot
        for (int i = 0; i < reflist.size(); ++i)
        {
            id.initial = reflist.at(i);
            id.metaid = "";
            idList.push_back(id);    
        }

        if(checkptr) {
            BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("abstracts-retrieval-response.references.reference")) {
                boost::property_tree::ptree ptr = v.second;

                checkptr = ptr.get_child_optional("ce:doi");
                if(checkptr) {
                    reflist.at(i)->doi = ptr.get<string>("ce:doi");
                }

                checkptr = ptr.get_child_optional("sourcetitle");
                if(checkptr) {
                    reflist.at(i)->sourceTitle = ptr.get<string>("sourcetitle");
                }

                checkptr = ptr.get_child_optional("author-list.author");
                if(checkptr) {
                    int k = 0;
                    BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, ptr.get_child("author-list.author")){
                        boost::property_tree::ptree ptr2 = v2.second;

                        //if one author, they put a double of that in to force an array?
                        if(k > 0 && ptr2.get<string>("ce:indexed-name") == reflist.at(i)->authors.at(k-1))
                            break;
                        reflist.at(i)->authors.push_back(ptr2.get<string>("ce:indexed-name"));
                        k++;
                    }
                }

                checkptr = ptr.get_child_optional("scopus-id");
                if(checkptr) {
                    idList.at(i).metaid = ptr.get<string>("scopus-id");
                }
                i++;
            }
        }
        cout << "Received from Scopus: " << doi << endl;

        pthread_t threads[idList.size()];
        for (int i = 0; i < idList.size(); i++) {
            pthread_create(&threads[i], NULL, metaCall, &idList.at(i));
        }

        corRef* currentRef;

        for (int i = 0; i < idList.size(); i++) {
            pthread_join(threads[i], (void**)&currentRef);
            corRefList.push_back(currentRef);
        }
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    result* currentResult = new result;
	currentResult->initial = reflist;
	currentResult->corrected = corRefList;

	return (void*)currentResult;
}

void* metaCall(void* param)
{
    prepComp *compstruct = static_cast<prepComp*>(param);
    corRef* corCurRef = new corRef;

    string id = compstruct->metaid;
    if(id == "") {
         corCurRef->title = "";
         corCurRef->year = "";
         corCurRef->sourceTitle = "";
         corCurRef->volume = "";
         corCurRef->issue = "";
         corCurRef->pageStart = "";
         corCurRef->pageEnd = "";
         corCurRef->doi = "";
         return corCurRef;
    }


    string metaurl = "http://api.elsevier.com/content/abstract/scopus_id/" + id + "?apiKey=eb697e3061f267d5945a3bc3c959874a&view=META&httpAccept=application/json";

    std::stringstream ss;
    curlpp::Easy c;

    c.setOpt( new curlpp::options::Url( metaurl ) );
    c.setOpt( new curlpp::options::FollowLocation( "true" ) );
    ss << c;  

    boost::property_tree::ptree cpt;
    boost::property_tree::read_json(ss, cpt);

    // Perform a search here on the reference to make sure the information we have from the API is correct
    corCurRef = new corRef;
    corCurRef->title = "";
    corCurRef->year = "";
    corCurRef->sourceTitle = "";
    corCurRef->volume = "";
    corCurRef->issue = "";
    corCurRef->pageStart = "";
    corCurRef->pageEnd = "";
    corCurRef->doi = "";

    // Check all the fields
    boost::optional< boost::property_tree::ptree& > corTitle = cpt.get_child_optional("abstracts-retrieval-response.coredata.dc:title");
    boost::optional< boost::property_tree::ptree& > corAuth = cpt.get_child_optional("abstracts-retrieval-response.coredata.dc:creator.author.ce:indexed-name");
    boost::optional< boost::property_tree::ptree& > corYear = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:coverDate");
    boost::optional< boost::property_tree::ptree& > corSourceTitle = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:publicationName");
    boost::optional< boost::property_tree::ptree& > corVolume = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:volume");
    boost::optional< boost::property_tree::ptree& > corIssue = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:issueIdentifier");
    boost::optional< boost::property_tree::ptree& > corPageStart = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:startingPage");
    boost::optional< boost::property_tree::ptree& > corPageEnd = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:endingPage");
    boost::optional< boost::property_tree::ptree& > corDoi = cpt.get_child_optional("abstracts-retrieval-response.coredata.prism:doi");

    if(corTitle || corYear || corSourceTitle || corVolume || corIssue || corPageStart || corPageEnd || corDoi)
    compstruct->initial->status = "Ok";

    if (corTitle) {
        string newTitle = cpt.get<string>("abstracts-retrieval-response.coredata.dc:title");
        if (compstruct->initial->title != newTitle) {
            corCurRef->title = newTitle;
            compstruct->initial->status = "Wrong";
        }
    }

    if (corAuth) {
        string newTitle = cpt.get<string>("abstracts-retrieval-response.coredata.dc:creator.author.ce:indexed-name");
        if (!(std::find(compstruct->initial->authors.begin(), compstruct->initial->authors.end(), newTitle) != compstruct->initial->authors.end())) {
            corCurRef->authors.push_back(newTitle);
            compstruct->initial->status = "Wrong";
        }
    }

    if (corYear) {
        string actualYear = cpt.get<string>("abstracts-retrieval-response.coredata.prism:coverDate");
        std::vector<string> strs;
        boost::split(strs, actualYear, boost::is_any_of("-"));
        string newYear = strs[0];

        if (compstruct->initial->year != newYear) {
            corCurRef->year = newYear;
            compstruct->initial->status = "Wrong";
        }
    }

    if (corSourceTitle) {
        string newSourceTitle = cpt.get<string>("abstracts-retrieval-response.coredata.prism:publicationName");
        if (compstruct->initial->sourceTitle != newSourceTitle) {
            corCurRef->sourceTitle = newSourceTitle;
            compstruct->initial->status = "Wrong";
        }
    }

    if (corVolume) {
        string newVolume = cpt.get<string>("abstracts-retrieval-response.coredata.prism:volume");
        if (compstruct->initial->volume != newVolume) {
            corCurRef->volume = newVolume;
            compstruct->initial->status = "Wrong";
        }
    }

    if (corIssue) {
        string newIssue = cpt.get<string>("abstracts-retrieval-response.coredata.prism:issueIdentifier");
        if (compstruct->initial->issue != newIssue) {
            corCurRef->issue = newIssue;
            compstruct->initial->status = "Wrong";
        }
    }

    if (corPageStart) {
        string newPageStart = cpt.get<string>("abstracts-retrieval-response.coredata.prism:startingPage");
        if (compstruct->initial->pageStart != newPageStart) {
            corCurRef->pageStart = newPageStart;
            compstruct->initial->status = "Wrong";
        }
    }

    if (corPageEnd) {
        string newPageEnd = cpt.get<string>("abstracts-retrieval-response.coredata.prism:endingPage");
        if (compstruct->initial->pageEnd != newPageEnd) {
            corCurRef->pageEnd = newPageEnd;
            compstruct->initial->status = "Wrong";
        }
    }

    if (corDoi) {
        string newDoi = cpt.get<string>("abstracts-retrieval-response.coredata.prism:doi");
        if (compstruct->initial->doi != newDoi) {
            corCurRef->doi = newDoi;
            compstruct->initial->status = "Wrong";
        }
    }

    return corCurRef;
}

