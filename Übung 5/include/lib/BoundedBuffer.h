#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include"thread/Activity.h"
#include"thread/ActivityScheduler.h"
#include"lib/Queue.h"
#include"interrupts/IntLock.h"
#include"io/PrintStream.h"
#include"sync/KernelLock.h"
#include"sync/Monitor.h"
#include "user/Environment.h"
#include "device/CPU.h"

extern Monitor monitor;

extern PrintStream out;

extern CPU cpu;

extern ActivityScheduler scheduler;

/** Diese Klasse stellt einen begrenzten synchronisierten
 *  Puffer dar, welcher von Interruptbehandlungsroutinen
 *  gef�llt werden kann.
 *
 *  Die Klasse ist eine Templateklasse. Das bedeutet f�r euch
 *  erstmal nur, das ihr alle Methoden hier im Header implementieren
 *  m�sst.
 */
template<typename T,unsigned size>
class BoundedBuffer {
public:

	BoundedBuffer()
	{
        for (unsigned i = 0; i < size; i++) {
            this -> buffer[i] = T();
        }
        this -> inPointer = 0;
        this -> outPointer = 0;
	}

	/** Diese Methode wird vom Interrupthandler aufgerufen, um
	 *  ein etwas in den Puffer zu schreiben. Ist der Puffer voll,
	 *  werden die zu schreibenden Daten verworfen.
	 *  Prozesse die auf eine Eingabe warten m�ssen hier geweckt werden.
	 */
	void add(T& elem)
	{   
        while(keyboardListSize != 0) {
            Activity* rapunzel = (Activity*) keyboardList.dequeue();
            keyboardListSize--;
            if (rapunzel) {
                rapunzel -> wakeup();
            }
        }
        
        if (!bufferIsFull()){
            this -> buffer[inPointer] = elem;
            elemInBuffer++;
            incInPointer();
        }
    }

	/** Diese Methode wird von Prozessen aufgerufen, um Daten aus dem
	 *  Puffer zu lesen. Ist dieser leer wird der lesende Prozess schlafen
	 *  gelegt. Achtet hier besonders auf die Synchronisierung.
	 */
	T get()
    {   
        cpu.disableInterrupts();
        
        if (this -> bufferIsEmpty()) {
            Activity* active = (Activity*) scheduler.active();
            this -> keyboardList.enqueue(active);
            this -> keyboardListSize++;
            cpu.enableInterrupts();
            scheduler.suspend();
            cpu.disableInterrupts();
        }
        
        T output = this -> buffer[outPointer];
        elemInBuffer--;
        incOutPointer();
        
        cpu.enableInterrupts();
        
        return output;
	}
	
    bool bufferIsEmpty() {
        return (elemInBuffer == 0);
    }

private:
	T buffer[size];
    
    Queue keyboardList;
    int keyboardListSize = 0;
    
    // zeigt auf erstes freies Element im Array
    unsigned inPointer;
    
    // zeigt auf erstes belegtes Element im Array
    unsigned outPointer;
    
    unsigned elemInBuffer = 0;
    
    
	void incInPointer() {
        this -> inPointer = (this -> inPointer + 1) % size;
    }
    
    void incOutPointer() {
        this -> outPointer = (this -> outPointer + 1) % size;
    }

    bool bufferIsFull() {
        return (elemInBuffer == size);
    }
};

#endif

