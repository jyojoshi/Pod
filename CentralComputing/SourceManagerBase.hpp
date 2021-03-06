#ifndef SOURCE_MANAGER_BASE_HPP
#define SOURCE_MANAGER_BASE_HPP

#include "Utils.h"
#include "Event.hpp"
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

#ifdef SIM
#include <fstream>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wctor-dtor-privacy"
#include "gtest/gtest.h"
#pragma GCC diagnostic pop
#endif

using namespace Utils;

template <long long DelayInUsecs, class Data, bool DataEvent >
class SourceManagerBase {

  public:
    std::shared_ptr<Data> Get() {
      mutex.lock();
      std::shared_ptr<Data> ret = data;
      mutex.unlock();
      return ret;
    }

    void initialize() {

      #ifdef SIM
        // Get name of current test case 
        const ::testing::TestInfo* const test_info = 
          ::testing::UnitTest::GetInstance()->current_test_info();
        // Create path to file // test_info->name() and test_info->test_case_name()
        std::string file =  std::string("tests/data/") 
                            + test_info->name() + std::string("/") 
                            + name() + std::string(".txt");
        test_data.open(file);
        initialized_correctly = test_data.is_open();
      #else
        // Initialize the source manager
        initialized_correctly = initialize_source();
      #endif

      closing.reset();
      if(initialized_correctly){
        // If initialized correcly, setup the worker
        
        #ifdef SIM
          data = read_test_data();
        #else
          data = refresh();
        #endif

        running.store(true);

        // I don't know how to start a thread using a member function, but I know how to use lambdas so suck it C++
        worker = std::thread( [&] { refresh_loop(); } );
      }
      else{

        // Did not setup correctly. Print error and set garbage data
        #ifdef SIM
          print(LogLevel::LOG_ERROR, "Failed to initialize: %s. Can't open file: %s\n", name().c_str(), file.c_str());
        #else
          print(LogLevel::LOG_ERROR, "Failed to initialize: %s. Not running worker thread\n", name().c_str());
        #endif
        running.store(false);

        // Set garbage data
        data = std::make_shared<Data>();
        memset(data.get(), (uint8_t)0, sizeof(Data));

      }
    }

    
    void stop() {
      if(initialized_correctly){
        running.store(false);
        closing.invoke();

        if(DataEvent){
          data_event.invoke();
        }

        worker.join();
        stop_source();
        #ifdef SIM
          test_data.close();
        #endif
      }
    }

    void data_event_wait(){
      data_event.wait();
    }
    void data_event_reset(){
      data_event.reset();
    }

    bool is_running(){
      return running.load();
    }

  private:
    // Init and Stop functions can setup devices/ file I/O
    // Stop will only be called if init returns true
    virtual bool initialize_source() = 0;
    virtual void stop_source() = 0;

    virtual std::string name() = 0;
    
    virtual std::shared_ptr<Data> refresh() = 0; //constructs a new Data object and fills it in

    void refresh_loop() {
      while(running.load()) {
        #ifndef SIM
          std::shared_ptr<Data> new_data = refresh();
        #else
          std::shared_ptr<Data> new_data = read_test_data();
        #endif
        mutex.lock();
        data = new_data;
        mutex.unlock();
        
        if(DataEvent) {
          data_event.invoke();
        }

        closing.wait_for(DelayInUsecs);
      }
    }

    #ifdef SIM
    std::shared_ptr<Data> read_test_data(){
      std::shared_ptr<Data> d = std::make_shared<Data>();
      memset(d.get(), (uint8_t)0, sizeof(Data));
      return d;
    }
    std::ifstream test_data;  
    #endif

    std::shared_ptr<Data> data;
    std::mutex mutex;
    std::atomic<bool> running;
    Event closing;
    Event data_event;
    std::thread worker;
    bool initialized_correctly;
};
#endif
