/*
 * JailParameters.h
 *
 *  Created on: March 1, 2019
 *      Author: Babak
 */

#ifndef SRC_JAILPARAMETERS_H_
#define SRC_JAILPARAMETERS_H_

#include <cmath>

namespace TransModel {

struct JailParameters {

    bool is_in_jail;
    double time_of_jail, serving_time;
    float age_at_jail;
    double time_since_jailed=0; 
    int accumulative_injail_count;  //accumulative number of times agent has been jailed
    double accumulative_time_in_jail; //accumulative time agent spent time in jail 

    JailParameters() : is_in_jail(false),
            accumulative_injail_count(NAN),
            accumulative_time_in_jail(0),
            time_of_jail(NAN),
            serving_time(NAN),
            age_at_jail(NAN),
            time_since_jailed(NAN)
            {}
    
    //JailParameters(bool in_jail = false): in_jail(in_jail) {}
            //bool in_jail;
    
    /**
     * Debugging helper function to print the jail record of a person  
    */ 
    void printJailRecord(double current_time) {
        //std::cout << "JailParameters.h:: printJailRecord is called " << std::endl;
        std::cout << "-----------Jail Record (@time: " <<  current_time << ")------------" << std::endl;
        std::cout << "accumulative_injail_count= " <<  accumulative_injail_count << std::endl;
        std::cout << "accumulative_time_in_jail= " <<   (accumulative_time_in_jail + (current_time - time_of_jail)) << std::endl;
        std::cout << "isJailed?= " <<  is_in_jail << std::boolalpha << std::endl;
        std::cout << "time_of_jail= " << time_of_jail;
        std::cout << ", age_at_jail= " << age_at_jail << std::endl;
        std::cout << "serving_time= " << serving_time;
        std::cout << ", time_since_jailed= " << (current_time - time_of_jail) << std::endl;
        std::cout << "----------------------------------------" << std::endl;
    }
};


} /* namespace TransModel */

#endif /* SRC_JAILPARAMETERS_H_ */