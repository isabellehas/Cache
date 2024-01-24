// CS472 Project 2 - Caches
// Yiwei Zhao

#include <iostream>
using namespace std;

class Cache
{
public:
    static const int slot_pos = 0; // slot position in cache structure
    static const int valid_bit_pos = 1; // valid bit position in cache structure
    static const int tag_pos = 2; // tag position in cache structure
    static const int dirty_bit_pos = 3; // dirty bit position in cache structure
    static const int data_pos = 4; // data start position in cache structure
    static const int cache_size = 16; // data size in cache structure
    static const int n = 20; //column number of cache structure
    static const int mainmemory = 2048; // size of main memory
    
    int cache[cache_size][n];
    int MM[mainmemory];
    
    Cache();
    void set_address(int addressinput);
    void get_tag();
    void set_tag();
    void get_offset();
    void set_valid_bit();
    void set_beg();
    void set_end();
    void get_slot();
    void copy_data();
    void set_dirty_bit();
    void read_cache();
    void write_cache();
    void display_cache();
    void update_mainmem();
    
private:
    int tag_no;
    int address;
    int offset;
    int block_beg;
    int block_end;
    int slot_no;
    int new_address;
};

int main()
{
    Cache cacheSim;
    
    while(!cin.eof() && cin.good())
    {
        cout << "\nWhat would you like to do? (R)ead, (W)rite, (D)isplay cache? ";
        char ans;
        cin >> ans;
        
        if(cin.eof())
        {
            cout << "Good Bye. \n" << endl;
        }
        else
        {
            if(ans == 'R')
            {
                cacheSim.read_cache();
            }
            else if(ans == 'W')
            {
                cacheSim.write_cache();
            }
            else
            {
                cacheSim.display_cache();
            }
        }
    }
    
    return 0;
    
}

// initialize cache and main memory
Cache::Cache()
{
    int mem_cnt = 0;
    
    while(mem_cnt < mainmemory)
    {
        for(short i = 0; i <= 0xFF; i++)
        {
            MM[mem_cnt] = i;
            mem_cnt++;
        }
    }
    
    int slot_cnt = 0;
    
    for (int i = 0; i < cache_size; i++)
    {
        cache[i][0] = slot_cnt;
        slot_cnt++;
        
        for(int j = 1; j < n; j++)
        {
            cache[i][j] = 0;
        }
    }
    
}

// set address
void Cache::set_address(int addressinput)
{
    address = addressinput;
}

// get slot
void Cache::get_slot()
{
    slot_no = (address & 0x0F0) >> 4;
}

// get tag
void Cache::get_tag()
{
    tag_no = address >> 8;
    
}

// set tag in cache
void Cache::set_tag()
{
    cache[slot_no][tag_pos] = tag_no;
}

//get offset
void Cache::get_offset()
{
    offset = address & 0x00F;
}

// set valid bit to 1
void Cache::set_valid_bit()
{
    cache[slot_no][valid_bit_pos] = 1;
}

// begin of block
void Cache::set_beg()
{
    block_beg = address & 0xFF0;
}

// end of block
void Cache::set_end()
{
    block_end = block_beg + 0x00F;
}

// set dirty bit
void Cache::set_dirty_bit()
{
    // if dirty bit is set to 1, set to 0.
    if(cache[slot_no][dirty_bit_pos] == 1)
    {
        cache[slot_no][dirty_bit_pos] = 0;
    }
    // if dirty bit is set to 0, set to 1
    else
    {
        cache[slot_no][dirty_bit_pos] = 1;
    }
}

// copy data fron main memory to cache
void Cache::copy_data()
{
    set_beg();
    set_end();
    
    int cnt_m = 0;
    
    for(int j = data_pos; j < n; j++)
    {
        cache[slot_no][j] = MM[block_beg + cnt_m];
        cnt_m++;
    }
}

void Cache::update_mainmem()
{
    new_address = (cache[slot_no][tag_pos] << 8) + (slot_no << 4);
    
    int count_update = 0 ;
    
    for(int i  = data_pos; i < n; i++)
    {
        MM[new_address + count_update] = cache[slot_no][i];
        count_update++;
    }
}

void Cache::read_cache()
{
    cout << "What address would you like to read? ";
    short addressinput;
    cin >> hex >> addressinput;
    
    while(addressinput > 0x7ff)
    {
        cout << "The address is invalid. Please try an address no greater than 0x7ff: ";
        cin >> hex >> addressinput;
    }
    
    if(addressinput <= 0x7ff)
    {
        set_address(addressinput);
        get_slot();
        get_tag();
        get_offset();
        
        // Cache Hit
        if(cache[slot_no][tag_pos] == tag_no && cache[slot_no][valid_bit_pos] == 1)
        {
            cout << "\nAt that byte there is the value " << hex << cache[slot_no][data_pos + offset] << " (Cache Hit)\n";
        }
        // Cache Miss
        else
        {
            if(cache[slot_no][dirty_bit_pos] == 1)
            {
                update_mainmem();
                
                set_valid_bit();
                set_tag();
                set_dirty_bit();
                
                copy_data();
            
                cout << "\nAt that byte there is the value " << hex << cache[slot_no][data_pos + offset] << " (Cache Miss)\n";

            }
            else
            {
                set_valid_bit();
                set_tag();
                
                copy_data();

                cout << "\nAt that byte there is the value " << hex << cache[slot_no][data_pos + offset] << " (Cache Miss)\n";
            }
        }
    }
}

void Cache::write_cache()
{
    cout << "What address would you like to write to? ";
    int addressinput;
    cin >> hex >> addressinput;
    
    set_address(addressinput);
    
    while(addressinput > 0x7ff)
    {
        cout << "The address is invalid. Please try an address no greater than 0x7ff: ";
        cin >> hex >> addressinput;
    }
    
    if(addressinput <= 0x7ff)
    {
        get_slot();
        get_tag();
        get_offset();
        
        // Cache Hit
        if(cache[slot_no][tag_pos] == tag_no && cache[slot_no][valid_bit_pos] == 1)
        {
            cout << "\nWhat data would you like to write at that address? ";
            int datainput;
            cin >> hex >> datainput;
            
            cache[slot_no][data_pos + offset] = datainput;
            if(cache[slot_no][dirty_bit_pos] == 0)
            {
                set_dirty_bit();
            }
            
            cout << "Value " << datainput << " has been written to address " << addressinput << "." << " (Cache Hit)\n";
        }
        // Cache Miss
        else
        {
            // dirty bit = 1, update main memory
            if(cache[slot_no][dirty_bit_pos] == 1)
            {
                update_mainmem();

                set_valid_bit();
                set_tag();
                set_dirty_bit();
                
                copy_data();
                
                cout << "What data would you like to write at that address? ";
                int datainput;
                cin >> hex >> datainput;
                
                cache[slot_no][data_pos + offset] = datainput;
                set_dirty_bit();
                
                cout << "Value " << datainput << " has been written to address " << addressinput << "." << " (Cache Miss)\n";
            }
            else
            {
                set_valid_bit();
                set_tag();
                
                copy_data();
                
                cout << "What data would you like to write at that address? ";
                int datainput;
                cin >> hex >> datainput;
                
                cache[slot_no][data_pos + offset] = datainput;
                set_dirty_bit();
                
                cout << "Value " << datainput << " has been written to address " << addressinput << "." << " (Cache Miss)\n";
            }
        }
    }
}

void Cache::display_cache()
{
    cout << "Slot Valid Tag Dirty Data\n";
    
    for(int i = 0; i < cache_size; i++)
    {
        cout <<  hex << "\n";
        for(int j = 0; j < n; j++)
        {
            cout << cache[i][j] << " ";
        }
        
        //cout <<  hex << "\n";
    }
}
