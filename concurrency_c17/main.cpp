#include <vector>
#include <thread>
#include <future>
#include <chrono>
#include <numeric>
#include <cmath>
#include <functional>
#include <iostream>
using namespace std;


int my_int {};
int my_int2 = 0;
int my_int3 = {};
int my_int4;

void accum( vector<int>::iterator first, vector<int>::iterator last, promise<int> promise_of_int, int initial_sum ) {  // todo: try adding const and ref
    promise_of_int.set_value( std::accumulate( first, last, initial_sum ));
}
int accum_no_promise( vector<int>::iterator first, vector<int>::iterator last, int initial_sum ) {  // todo: try adding const and ref
    return std::accumulate( first, last, initial_sum );
}
void work_that_just_sets_completion ( promise<void> barrier_only_promise ) {
    this_thread::sleep_for( chrono::seconds(1) );
    barrier_only_promise.set_value();  // seriously, can you set a void??
}
int main() {

/*    // try {
    // }  catch (...) {
            // cout << "caught error: "+std::to_string(__LINE__)+":"+__FUNCTION__<<endl;
    // }
*/
    vector 			vec1 				{ 0,1,2,3,4,5,6,7,8,9 };
    promise<int> 	work_promise		{ };  										// int is required.
    future<int>		work_future    		{ work_promise.get_future() };
    thread 			work_thread   		{ accum, vec1.begin(), vec1.end(), std::move( work_promise ), 100};  // can't copy a promise??
    // work_future.wait();																		   // not needed, get() will block.
    int const 		result 				{ work_future.get() }; cout << "work:"<<result<< endl;
    work_thread.join();

{    // NO a promise can't be reused, but a future can??
    promise<int> 	work2_promise		{ };  										// int is required.
    work_future = 	work2_promise.get_future();
    thread 			work2_thread 		{ accum, vec1.begin(), vec1.end(), std::move( work2_promise ), 200};
    int const 		result2 			{ work_future.get() }; cout << "work2:"<<result2<< endl;
    work2_thread.join();
}

{    promise<void> 	work3_promise		{ };  // promise of nothing, except that we will set() it with nothing.
    future<void> 	work3_future 		{ work3_promise.get_future() };
    thread 			work3_thread 		{ work_that_just_sets_completion, std::move( work3_promise ) };
    work3_future.wait();				// here the wait becomes important.
    work3_thread.join();
    }

    // Packaged_Task avoids the need to set_value(), instead we just return the value.
    packaged_task<int()> promising_ptask4 { []{return 42;} };
    future<int> 	work4_future 		{ promising_ptask4.get_future() };
    promising_ptask4();                   // instead of: thread	work4_thread  { std::move( promising_task ) };
    int const		result4 		    { work4_future.get() }; cout << "work4:"<<result4<< endl;

{    packaged_task<int()> promising_ptask5 { std::bind( accum_no_promise, vec1.begin(), vec1.end(), 500 ) };
    future<int> 	work5_future 		{ promising_ptask5.get_future() };
    promising_ptask5();
    int const		result5 		    { work5_future.get() }; cout << "work5:"<<result5<< endl;
}

{   packaged_task<int(vector<int>::iterator, vector<int>::iterator, int)> promising_ptask6 { accum_no_promise };
    future<int> 	work6_future 		{ promising_ptask6.get_future() };
    thread 			work6_thread 		{ std::move( promising_ptask6 ), vec1.begin(), vec1.end(), 600};
    work6_thread.join();
    int const		result6 		    { work6_future.get() }; cout << "work5:"<<result6<< endl;
}
    cout << "###" << endl;
    return 0;
}
