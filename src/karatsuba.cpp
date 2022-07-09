#include <iostream>
#include <assert.h>
#include <sstream>

#include "bigint.hpp"
#include "test.hpp"

template<typename Word>
void tests() {
#ifdef DNDEBUG
    return;
#endif
    std::cout << "Running tests for " << sizeof(Word) << " byte word" << std::endl;

    using INT = my::big_uint<Word>;
    {
        INT a {"0"};
        CHECK_EQ(a.to_string(), "0x0");
    }
    {
        INT a {"123"};
        CHECK_EQ(a.to_string(), "0x7b");
    }
    {
        INT a {"256"};
        CHECK_EQ(a.to_string(), "0x100");
    }
    {
        INT a {"3457"};
        CHECK_EQ(a.to_string(), "0xd81");
    }
    {
        INT a {"8092539"};
        CHECK_EQ(a.to_string(), "0x7b7b7b");
    }
    {
        INT a {"10"};
        INT b {"20"};
        INT c {"30"};
        CHECK_EQ(a + b, c);
    }
    {
        INT a {"63216312623"};
        CHECK_EQ(a.to_string(),  "0xeb7fc612f");
    }
    {
        INT a {"345421345531"};
        CHECK_EQ(a.to_string(), "0x506cb772fb");
    }
    {
        INT a {"408637658154"};
        CHECK_EQ(a.to_string(), "0x5f24b3d42a");
    }
    {    
        INT a {"0"};
        INT b {"0"};
        INT expected {"0"};
        INT c = a + b;
        CHECK_EQ(c, expected);
    }
    {
        INT a {"255"};
        INT b {"1"};
        INT c = a + b;
        INT expected {"256"};
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"623"};
        INT b {"531"};
        INT expected {"1154"};
        INT c = a + b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"63216312623"};
        INT b {"345421345531"};
        INT expected {"408637658154"};
        INT c = a + b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"12345678965216435645"};
        INT b {"683411624631216543135135123"};
        INT expected {"683411636976895508351570768"};
        INT c = a + b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"0"};
        INT b {"0"};
        INT expected {"0"};
        INT c = a - b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"10"};
        INT b {"10"};
        INT expected {"0"};
        INT c = a - b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"13"};
        INT b {"5"};
        INT expected {"8"};
        INT c = a - b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"260"};
        INT b {"10"};
        INT expected {"250"};
        INT c = a - b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"6531654653"};
        INT b {"6500000000"};
        INT expected {"31654653"};
        INT c = a - b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"683411636976895508351570768"};
        INT b {"683411624631216543135135123"};
        INT expected {"12345678965216435645"};
        INT c = a - b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"0"};
        INT b {"0"};
        INT expected {"0"};
        INT c = a * b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"7"};
        INT b {"3"};
        INT expected {"21"};
        INT c = a * b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"200"};
        INT b {"10"};
        INT expected {"2000"};
        INT c = a * b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"256"};
        INT b {"256"};
        INT expected {"65536"};
        INT c = a * b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"5678"};
        INT b {"1069"};
        INT expected {"6069782"};
        INT c = a * b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"1234"};
        INT b {"5678"};
        INT expected {"7006652"};
        INT c = a * b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"6538645315"};
        INT b {"96543684210"};
        INT expected {"631264908452555976150"};
        INT c = a * b;
        CHECK_EQ(c, expected);
    }
    {    
        INT a {"6538645315"};
        INT b {"96543684210"};
        INT expected {"631264908452555976150"};
        INT c = a * b;
        CHECK_EQ(c, expected);
    }
    {
        INT a {"3141592653589793238462643383279502884197169399375105820974944592"};
        INT b {"2718281828459045235360287471352662497757247093699959574966967627"};
        INT expected {"8539734222673567065463550869546574495034888535765114961879601127067743044893204848617875072216249073013374895871952806582723184"};
        INT c = a * b;
        CHECK_EQ(c, expected);
    }
}


int main() {
    tests<std::uint8_t>();
    tests<unsigned short>();
    tests<unsigned int>();
    tests<unsigned long>();

    const my::big_uint<std::uint8_t> A {"3141592653589793238462643383279502884197169399375105820974944592"};
    const my::big_uint<std::uint8_t> B {"2718281828459045235360287471352662497757247093699959574966967627"};

    std::cout << "Solving:" << '\n';
    std::cout << std::setw(110) << A <<'\n';
    std::cout << 'x' << std::setw(109) << B <<'\n';
    for(std::size_t i=0; i<110; ++i) std::cout << "-";
    std::cout <<'\n'<< std::setw(110) << A * B << std::endl;
}