/*
 * ActivityScheduler: 	Diese Klasse implementiert die Zustandsverwaltung
 * 			f�r Activities
 *
 */

#include "lib/Queue.h"
#include "thread/Activity.h"
#include "thread/Dispatcher.h"
#include "thread/Scheduler.h"
#include "thread/ActivityScheduler.h"
#include "device/CPU.h"
#include "sync/KernelLock.h"
#include "sync/Monitor.h"
#include "io/PrintStream.h"

extern PrintStream out;

extern Monitor monitor;

extern CPU cpu;

	/* Suspendieren des aktiven Prozesses
	 * Der korrekte Ausfuehrungszustand ist zu setzen
	 * und danach der naechste lauffaehige Prozess zu aktivieren.
	 */
	void ActivityScheduler::suspend()	{
        
        KernelLock lock;
        
        Activity* active = (Activity*) scheduler.active();
        
        active -> changeTo(Activity :: BLOCKED);
        
        scheduler.reschedule();
        
	}

	/* Explizites Terminieren des angegebenen Prozesses
	 * Der Prozesszustand ist korrekt zu setzen
	 * und der Prozess aus der Ready-Liste des Schedulers
	 * zu entfernen. Falls der aktive Prozess zerstoert wird,
	 * ist dem naechsten lauffaehigen Prozess die CPU
	 * zuzuteilen.
	 */
	void ActivityScheduler::kill(Activity* act) {
        
        KernelLock lock;
        
        bool laeuft;

        if (act -> isRunning()) {
            laeuft = true;
        } else {
            laeuft = false;
        }
        
        act -> changeTo(Activity :: ZOMBIE);
        
        if (laeuft) {
            scheduler.reschedule();
        } else {
            scheduler.remove(act);
        }
	}

	/* Terminieren des aktiven Prozesses,
	 * und Wechsel zum naechsten lauffaehigen Prozess
	 */
	void ActivityScheduler::exit() {
        
        KernelLock lock;
        
		Activity* active = (Activity*) scheduler.active();
        active -> changeTo(Activity :: ZOMBIE);
        
        this -> reschedule();
	}

	/* Der aktive Prozess ist, sofern er sich nicht im Zustand
	 * Blocked oder Zombie befindet, wieder auf die Ready-Liste
	 * zu setzen. Danach ist "to" mittels dispatch die Kontrolle
	 * zu �bergeben.
	 */
	void ActivityScheduler::activate(Schedulable* to) {
        
        Activity* active = (Activity*) scheduler.active(); 
        Activity* next = (Activity*) to;

        // out.println("activate wird aufgerufen.");
        // for (int i = 0; i < 10000000; i++);
        
        if (activateBlocked) {
            return;
        }
        
        if ((active -> isRunning()) && (next == 0)) {
            return;
        }
        
        // wenn Zustand Running oder Ready, dann automatisch nicht im Zustand Blocked/Zombie
        if ((active -> isRunning()) ||  (active -> isReady())){
            if (next != active) {
                active -> changeTo(Activity :: READY);
                scheduler.schedule(active);
            } else {
                return;
            }
        }

        if(next == 0) {
            // out.println("next ist null in activate");
            while (next == 0) {
                
                activateBlocked = true;
                
                // interrupts kurz zulassen
                monitor.leave();
                cpu.halt();
                //CPU::halt();
                monitor.enter();
                
                next = (Activity*) readylist.dequeue();
            }
            activateBlocked = false;
            next -> changeTo(Activity :: RUNNING);
            dispatch(next);
        } else {
            // kein Prozesswechsel, wenn der zu aktivierende Prozess eh aktiv ist  
            if (next != active) {
                next -> changeTo(Activity :: RUNNING);
                dispatch(next);
            }
        }
	}
