#include "daisysp.h"
#include "daisy_patch.h"

#include <functional>
#include <string>

using namespace daisy;
using namespace daisysp;

namespace ev_seq {

    // Holds a seq parameter of variable type
    // Purely a base class
    class SeqParam{
        protected:
            SeqParam(){};
            virtual std::string getValStr() = 0;
            virtual int8_t  getValInt()     = 0;
            virtual uint8_t getValUint()    = 0;
            virtual bool    getValBool()    = 0;
        public:
            std::string title;
    };

    class BoolParam : public SeqParam {
        private:
            bool val;
        public:
            BoolParam(){
                title = "";
                b = true;
            };
            BoolParam(bool b, std::string t){
                title = t;
                val = b;
            };

            // For now we'll just use direct access instead of getters or setters, which currently don't do anything helpful
            //
            // bool getValBool(){
            //     return val;
            // };
            // bool setValBool(bool b){
            //     val b;
            // };
    };


    // Represents the state of one step; holds any number of any type parameters
    class SeqStep {
        private:
            std::vector<*SeqParam> params;
        public:
            SeqStep(){};
            SeqStep(uint8_t n){
                params = std::vector<*SeqParam>(n);
            };
    }


    
    class Seq {
    private:
        std::array
    public:

} // namespace ev_sequencer 