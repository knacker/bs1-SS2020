#ifndef CgaAttr_h
#define CgaAttr_h

/*
 * CgaAttr: 	Softwareprototyp fuer CGA Darstellungsattribute
 *		Hier braucht man Bitoperationen!
 *
 *		Da *alle* Operationen sehr einfach und kurz sind,
 *		duerft Ihr sie direkt in der Klasse
 *		als inline Methoden deklarieren.
 */

class CgaAttr {
private:
	enum AttrMaskAndShifts {
        
        FGPOSITION = 0,     // Position des Bits wo die Vordergrundfarbe im Byte anf�ngt
        BGPOSITION = 4,     // Position des Bits wo die Hintergrundfarbe im Byte anf�ngt
        BLPOSITION = 7,     // Position des Bits wo der Blinker im Byte anf�ngt
        
        FGNUMBITS = 4,      // L�nge der Vordergrundfarbe in Bits
        BGNUMBITS = 3,      // L�nge der Hintergrundfarbe in Bits
        BLNUMBITS = 1       // L�nge des Blinkers in Bits
	};


public:
	/** 	Diese Aufz�hlung enth�lt die CGA-Farben als Farbkonstanten
	 *	Tragt hier *alle* CGA Farben mit den richtigen Werten ein
	 */
	enum Color {
        
        BLACK = 0,
        BLUE = 1,
        GREEN = 2,
        CYAN = 3,
        RED = 4,
        MAGENTA = 5,
        BROWN = 6,
        LIGHT_GRAY = 7,
        GRAY = 8,
        LIGHT_BLUE = 9,
        LIGHT_GREEN = 10,
        LIGHT_CYAN = 11,
        LIGHT_RED = 12,
        LIGHT_MAGENTA = 13,
        YELLOW = 14,
        WHITE = 15
        
        /*
         * Wichtig:
         * Betrachtet wird nur der zweite Byte (1 Byte = 8 Bits)
         * Bits 0 - 3 = Vordergrundfarbe
         * Bits 4 - 6 = Hintergrundfarbe
         * Bit 7 = Blinker
         * Gelesen wird dabei von rechts nach links!
         * ACHTUNG! Die Hintergrundfarbe kann maximal 3 Bits speichern, weswegen nur die Farben 0 bis 7 genutzt werden k�nnen
         */ 
	};


	/** Konstruktor. Erzeugt ein CgaAttr-Objekt mit den uebergebenen Werten f�r
	  * Vorder- und Hintergrundfarbe. Werden keine Parameter uebergeben,
	  * so werden die Defaultwerte (Vordergrund weiss, Hintergrund schwarz, Blinken deaktiviert)
	  * verwendet.
	  */
	CgaAttr(Color fg=WHITE, Color bg=BLACK, bool blink=false)
    {
        // Hintergrundfarbe darf nicht gr��er als 7 sein (Anzahl der Bits darf nicht 3 �bersteigen)
        // und wird somit auf schwarz gesetzt
        if ((int) bg > 7) {     
            bg = BLACK;                          
        }                                        
        
        this -> bitInformation = ((blink | bg) << FGNUMBITS) | fg;
	}

	// setzen der Schriftfarbe
	void setForeground(Color col)
	{
        // �berschreiben der letzten 4 Bits um Vordergrundfarbe zu clearen
        // Danach kann eine neue gesetzt werden
        int zwischenwert = (this -> bitInformation) &= ~(FGNUMBITS << FGPOSITION);
        this -> bitInformation = zwischenwert | (col << FGPOSITION);
	}

	// setzen der Hintergrundfarbe
	void setBackground(Color col)
	{
        // Hintergrundfarbe darf nicht gr��er als 7 sein (Anzahl der Bits darf nicht 3 �bersteigen)
        if (col > 7) {
            col = Color(col);
        }
        
        // �berschreiben der 3 Bits von der Hintergrundgarbe um diese zu clearen
        // Danach kann eine neue gesetzt werden
        int zwischenwert = (this -> bitInformation) &= ~(BGNUMBITS << BGPOSITION);
        this -> bitInformation = zwischenwert | (col << BGPOSITION);
	}

	// setzen blinkender/nicht blinkender Text
	void setBlinkState(bool blink)
	{
        // �berschreiben der letzten 4 Bits um Vordergrundfarbe zu clearen
        // Danach kann eine neue gesetzt werden
        int zwischenwert = (this -> bitInformation) &= ~(BLNUMBITS << BLPOSITION);
        this -> bitInformation = zwischenwert | (blink << BLPOSITION);
	}

	// setzen aller Attribute
	void setAttr(CgaAttr attr)
	{
        this -> bitInformation = attr.bitInformation;
	}

	// ermitteln der Schriftfarbe
	Color getForeground()
	{
        // L�schen die Bits f�r den Blinker und der Hintergrundfarbe um nur die Vordergrundfarbe zu erhalten
        int foregroundColor = (this -> bitInformation) &= ~((BGNUMBITS + BLNUMBITS) << BGPOSITION);
        
        return (Color) foregroundColor;
	}

	// ermitteln der Hintergrundfarbe
	Color getBackground()
	{
        // L�schen die Bits f�r den Blinker und der Vordergunrdfarbe um nur die Hintergrundfarbe zu erhalten
        int backgroundColor = (this -> bitInformation) &= ~(BLNUMBITS << BLPOSITION);
        int backgroundColor2 = backgroundColor >> FGNUMBITS;
        
        return (Color) backgroundColor2;
	}

	// ermitteln ob Blink-Flag gesetzt ist
	bool getBlinkState()
	{
        // Shiften die Bits f�r die Hintergrund- und Vordergrundfarbe nach rechts um nur den Blinker zu erhalten
        int blinkState = (this -> bitInformation) >> (FGNUMBITS + BGNUMBITS);
        
        return (bool) blinkState;
	}

private:
    
    // Speichern des 2. Bytes um die Attribute anzupassen
    int bitInformation;
    
};

#endif
