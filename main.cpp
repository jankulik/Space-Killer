#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Collision.h"

using namespace sf;
using namespace std;

int szerokosc = 1024;
int wysokosc = 639;

int score_int;
int score_kontrola = 0;

int level;

int zycie_int;

bool menu = true, gra = false, koniec = false, program = true, bBoss = false, bUstawienia = false;

const float strzal_co = 0.6f;
const float wrog_co = 3.75f;
const float boss_co = 22.0f;
float strzal_wroga_co = 1.07f;
float strzal_bossa_co = 1.07f;

float statek_speed = 300;
float pocisk_speed = 600;
float strzal_wroga_speed = 450;
float wrog_speed = 42;
float boss_speed = 45;

Clock deltaClock;
Time dt;

Clock zegar;
Time strzalGranica = seconds( strzal_co );
Time strzal;

Clock zegar_wroga;
Time wrogGranica = seconds( wrog_co );
Time wrog_spawn;

Clock zegar_bossa;
Time bossGranica = seconds( boss_co );
Time boss_spawn;

Clock zegar_strzalu_wroga;
Time strzal_wrogaGranica = seconds( strzal_wroga_co );
Time strzal_wroga_spawn;

Clock zegar_strzalu_bossa;
Time strzal_bossaGranica = seconds( strzal_bossa_co );
Time strzal_bossa_spawn;

string na_string( int liczba )
{
	ostringstream ss;
	ss << liczba;
	return ss.str();
}

struct strzal_struktura
{
    double x;
    double y;

    strzal_struktura( double f_x = 0.0, double f_y = 0.0 )
        : x( f_x )
        , y( f_y )
    { }
    
    void przesun_pocisk( double idzDoY )
    {       
		if( y > idzDoY )
        {
            y -= pocisk_speed * dt.asSeconds();
        }
    }

	bool poza_oknem()
	{
		if( y < -50 )
			return true;
		else
			return false;
	}

	Sprite zwroc_sprite( Sprite obiekt )
	{
		obiekt.setPosition( x, y );
		return obiekt;
	}
};

struct strzal_wroga
{
	 double x;
     double y;

    strzal_wroga( double f_x = 0.0, double f_y = 0.0 )
        : x( f_x )
        , y( f_y )
    { }
    
    void przesun_pocisk( double idzDoY )
    {       
		if( y < idzDoY )
        {
            y += strzal_wroga_speed * dt.asSeconds();
        }
    }

	bool poza_oknem()
	{
		if( y > wysokosc + 10 )
			return true;
		else
			return false;
	}

	Sprite zwroc_sprite( Sprite obiekt )
	{
		obiekt.setPosition( x, y );
		return obiekt;
	}
};

struct strzal_bossa
{
	 double x;
     double y;

    strzal_bossa( double f_x = 0.0, double f_y = 0.0 )
        : x( f_x )
        , y( f_y )
    { }
    
    void przesun_pocisk( double idzDoY )
    {       
		if( y < idzDoY )
        {
            y += strzal_wroga_speed * dt.asSeconds();
        }
    }

	bool poza_oknem()
	{
		if( y > wysokosc + 10 )
			return true;
		else
			return false;
	}

	Sprite zwroc_sprite( Sprite obiekt )
	{
		obiekt.setPosition( x, y );
		return obiekt;
	}
};

struct wrog_struktura
{
    double x;
    double y;

	int zycie;

    wrog_struktura( double f_x = 0.0, double f_y = 0.0, int zycie = 2.0 )
        : x( f_x )
        , y( f_y )
		,zycie( zycie )
    { }
    
    void przesun_wroga( double idzDoY )
    {
		if( y < idzDoY )
        {
            y += wrog_speed * dt.asSeconds(); 
        } 
		
		else if( y > idzDoY )
        {
            y -= wrog_speed * dt.asSeconds();
        }

    }

	bool poza_oknem()
	{
		if( y > wysokosc )
			return true;
		else
			return false;
	}

	Sprite zwroc_sprite( Sprite obiekt )
	{
		obiekt.setPosition( x, y );
		return obiekt;
	}

	bool czy_moge_strzelic()
	{
		if( strzal_wroga_spawn < strzal_wrogaGranica )
		{
			strzal_wroga_spawn += zegar_strzalu_wroga.restart();
			return false;
		}

		else
		{
			strzal_wroga_spawn = Time::Zero;
			return true;
		}
	}
};

struct boss_struktura
{
	double x;
    double y;

	int zycie;

	int set_zycie( int x ) { return zycie = x; }

	inline void set_x( double X ) { x = X; }

	inline void set_y( double Y ) { y = Y; }

	inline float rozmiar_x( Texture tekstura ) { return tekstura.getSize().x; }

	inline float rozmiar_y( Texture tekstura ) { return tekstura.getSize().y; }
    
    void przesun_bossa( double idzDoX, double idzDoY )
    {
		if( x < idzDoX )
        {
            x += boss_speed * dt.asSeconds();
        } 
   
		else if( x > idzDoX )
        {
            x -= boss_speed * dt.asSeconds();
        }

		if( y < idzDoY )
        {
            y += boss_speed * dt.asSeconds(); 
        } 
		
		else if( y > idzDoY )
        {
            y -= boss_speed * dt.asSeconds();
        }

    }

	Sprite zwroc_sprite( Sprite obiekt )
	{
		obiekt.setPosition( x, y );
		return obiekt;
	}

	void rysuj( RenderWindow & okno, Sprite tmp )
	{
        tmp.setPosition( x, y );
        okno.draw( tmp );
	}

	bool czy_kolizja( Sprite obiekt )
	{
		if( x > obiekt.getGlobalBounds().left &&
            y > obiekt.getGlobalBounds().top &&
            x < obiekt.getGlobalBounds().left + obiekt.getGlobalBounds().width &&
            y < obiekt.getGlobalBounds().top +  obiekt.getGlobalBounds().height )
		{
			return true;
		}

		else
			return false;
	}
};

int main()
{
	srand( time( NULL ) );

	typedef vector < strzal_struktura > Obiekty_Struktury_Strzalu;
    Obiekty_Struktury_Strzalu obiekty_struktury_strzalu;

	typedef vector < wrog_struktura > Obiekty_Struktury_Wroga;
    Obiekty_Struktury_Wroga obiekty_struktury_wroga;

	typedef vector < strzal_wroga > Obiekty_Struktury_Strzalu_Wroga;
    Obiekty_Struktury_Strzalu_Wroga obiekty_struktury_strzalu_wroga;

	typedef vector < strzal_bossa > Obiekty_Struktury_Strzalu_Bossa;
    Obiekty_Struktury_Strzalu_Bossa obiekty_struktury_strzalu_bossa;

	Texture tekstura_bossa;
    tekstura_bossa.loadFromFile( "boss.png" );
    Sprite boss;
    boss.setTexture( tekstura_bossa );

    Texture tekstura_tla;
    tekstura_tla.loadFromFile( "tlo.png" );
    Sprite tlo;
    tlo.setTexture( tekstura_tla );

	Texture tekstura_pocisku;
    tekstura_pocisku.loadFromFile( "pocisk.png" );
    Sprite pocisk;
    pocisk.setTexture( tekstura_pocisku );
	pocisk.setScale ( 0.5, 0.5 );

	Texture tekstura_wroga;
    tekstura_wroga.loadFromFile( "wrog.png" );
    Sprite wrog;
    wrog.setTexture( tekstura_wroga );
	
	Texture tekstura_pocisku_wroga;
    tekstura_pocisku_wroga.loadFromFile( "pocisk_wroga.png" );
    Sprite pocisk_wroga;
    pocisk_wroga.setTexture( tekstura_pocisku_wroga );

	Texture tekstura_pocisku_bossa;
	tekstura_pocisku_bossa.loadFromFile( "pocisk_bossa.png" );
	Sprite pocisk_bossa;
	pocisk_bossa.setTexture( tekstura_pocisku_bossa );

	Texture tekstura_statku;
    tekstura_statku.loadFromFile( "statek.png" );
    Sprite statek;
    statek.setTexture( tekstura_statku );
    statek.setPosition( szerokosc/2, 550 );

	Texture tekstura_zycia;
	tekstura_zycia.loadFromFile( "zycie.png" );
	Sprite zycie;
	zycie.setTexture( tekstura_zycia );

	Font czcionka;
	czcionka.loadFromFile( "Mecha.ttf" );

	boss_struktura boss_str;

while( program )
{
	if(menu)
	{
		RenderWindow menu_okno( VideoMode( szerokosc, wysokosc ), "Space killer" );

		cout << " JAK GRAC" << endl << " strzalki - prawo, lewo" << endl << " spacja - strzal";

		Text tytul( "Space killer", czcionka, 80 );
		tytul.setStyle( Text::Bold );
		tytul.setPosition( szerokosc/2 - tytul.getGlobalBounds().width/2, 20);

		const int ile = 2;

		Text tekst[ile];

		String opcje[] = { "Play", "Exit"};

		for( int i=0; i < ile; i++ )
		{
			tekst[i].setFont( czcionka );
		    tekst[i].setCharacterSize( 65 );
 
		    tekst[i].setString( opcje[i] );
		    tekst[i].setPosition( szerokosc/2 - tekst[i].getGlobalBounds().width/2,250+i*120);
		}

		while( menu_okno.isOpen() )
		{
			Event zdarzenie_menu;

			while( menu_okno.pollEvent( zdarzenie_menu ) )
			{
				if( zdarzenie_menu.type == Event::Closed )
				{
					menu_okno.close();
					program = false;
				}

				for( int i = 0; i < ile; i++ )
				{
					if ( Mouse::getPosition( menu_okno ).x > tekst[i].getGlobalBounds().left &&
                         Mouse::getPosition( menu_okno ).y > tekst[i].getGlobalBounds().top &&
                         Mouse::getPosition( menu_okno ).x < tekst[i].getGlobalBounds().left + tekst[i].getGlobalBounds().width &&
                         Mouse::getPosition( menu_okno ).y < tekst[i].getGlobalBounds().top +  tekst[i].getGlobalBounds().height )
				    {
                        tekst[i].setColor( Color::Cyan );
                    }

                    else
				    {
                       tekst[i].setColor( Color::White );
                    }
				}
			}

			if ( Mouse::getPosition( menu_okno ).x > tekst[0].getGlobalBounds().left &&
                 Mouse::getPosition( menu_okno ).y > tekst[0].getGlobalBounds().top &&
                 Mouse::getPosition( menu_okno ).x < tekst[0].getGlobalBounds().left + tekst[0].getGlobalBounds().width &&
                 Mouse::getPosition( menu_okno ).y < tekst[0].getGlobalBounds().top +  tekst[0].getGlobalBounds().height && Mouse::isButtonPressed( Mouse::Left ))
				    {
						gra = true;
                        menu = false;
						menu_okno.close();
                    }

			if ( Mouse::getPosition( menu_okno ).x > tekst[1].getGlobalBounds().left &&
                 Mouse::getPosition( menu_okno ).y > tekst[1].getGlobalBounds().top &&
                 Mouse::getPosition( menu_okno ).x < tekst[1].getGlobalBounds().left + tekst[1].getGlobalBounds().width &&
                 Mouse::getPosition( menu_okno ).y < tekst[1].getGlobalBounds().top +  tekst[1].getGlobalBounds().height && Mouse::isButtonPressed( Mouse::Left ))
				    {
                        menu_okno.close();
						program = false;
                    }

			menu_okno.clear();

			menu_okno.draw( tlo );

			menu_okno.draw( tytul );

			for(int i=0;i<ile;i++)
				menu_okno.draw( tekst[i] );

			menu_okno.display();
		}
	}

	if(gra)
	{
		RenderWindow gra_okno( sf::VideoMode( szerokosc, wysokosc ), "Space killer" );

		score_int = 0;
		
		zycie_int = 192;

		Text tekst_score( "SCORE: ", czcionka, 23 );
		tekst_score.setStyle( Text::Bold );
		tekst_score.setPosition( 2, 0 );

		string score;
		Text wyswietl_score;
		wyswietl_score.setFont( czcionka );
		wyswietl_score.setCharacterSize( 23 );
		wyswietl_score.setStyle( Text::Bold );
		wyswietl_score.setPosition( 62, 0 );

		zegar_bossa.restart();
		boss_spawn = Time::Zero;

		zegar_wroga.restart();
		wrog_spawn = Time::Zero;

		 while( gra_okno.isOpen() )
         {
		   int wylosowana = ( ( rand()% szerokosc - 60 ) +0 );

		   if( score_kontrola + 100 <= score_int )
		   {
			   score_kontrola = score_int;

			   if( strzal_wroga_co > 0.5f )
			   {
			        strzal_wroga_co -= 0.05f;
                    strzal_bossa_co -= 0.05f;
			        strzal_wrogaGranica = seconds( strzal_wroga_co );
			        strzal_bossaGranica = seconds( strzal_bossa_co );
			   }
		   }

		   score = na_string( score_int );
		   wyswietl_score.setString( score );

           Event zdarzenie;
           while( gra_okno.pollEvent( zdarzenie ) )
		   {
			   if( zdarzenie.type == Event::Closed )
			   {
                 gra_okno.close();
				 program = false;
			   }
           }

		   if( Keyboard::isKeyPressed( Keyboard::Left ) )
		   {
			   if( statek.getPosition().x > 0 )
	            {
					statek.move( -dt.asSeconds() * statek_speed, 0 );
	            }
		   }

           if( Keyboard::isKeyPressed( Keyboard::Right ) )
		   {
			     if( statek.getPosition().x < szerokosc - 63 )
			    {
					statek.move( dt.asSeconds() * statek_speed, 0 );
			    }
		   }

		   strzal += zegar.restart();
		   if( Keyboard::isKeyPressed( Keyboard::Space ) && strzal >= strzalGranica  )
		   {
			   obiekty_struktury_strzalu.push_back( strzal_struktura( statek.getPosition().x + 20, statek.getPosition().y ) );
			   strzal = Time::Zero;
           }

		   for( int i = 0; i < obiekty_struktury_strzalu.size(); i++ )
		   {
                obiekty_struktury_strzalu[i].przesun_pocisk( statek.getPosition().y - wysokosc );
           }

		   for( int i = 0; i < obiekty_struktury_strzalu.size(); i++)
		   {
			   if( obiekty_struktury_strzalu[i].poza_oknem() )
			   {
				   obiekty_struktury_strzalu.erase( obiekty_struktury_strzalu.begin() + i );
			   }
		   }

		   strzal_bossa_spawn += zegar_strzalu_bossa.restart();
		   if( strzal_bossa_spawn >= strzal_bossaGranica && bBoss )
		   {
			   obiekty_struktury_strzalu_bossa.push_back( strzal_bossa( boss_str.x + 10, boss_str.y + 40 ) );
			   obiekty_struktury_strzalu_bossa.push_back( strzal_bossa( boss_str.x + 61, boss_str.y + 40 ) );

			   strzal_bossa_spawn = Time::Zero;
		   }

		   for( int i = 0; i < obiekty_struktury_strzalu_bossa.size(); i++ )
		   {
			   obiekty_struktury_strzalu_bossa[i].przesun_pocisk( wysokosc );
		   }

		   for( int i = 0; i < obiekty_struktury_strzalu_bossa.size(); i++)
		   {
			   if( obiekty_struktury_strzalu_bossa[i].poza_oknem() )
			   {
				   obiekty_struktury_strzalu_bossa.erase( obiekty_struktury_strzalu_bossa.begin() + i );
			   }
		   }

		   for( int i = 0; i < obiekty_struktury_wroga.size(); i++ )
		   {
			   if( obiekty_struktury_wroga[i].czy_moge_strzelic() )
			   {
				   for( Obiekty_Struktury_Wroga::const_iterator i = obiekty_struktury_wroga.begin(); i != obiekty_struktury_wroga.end(); i++ )
				   {
					   obiekty_struktury_strzalu_wroga.push_back( strzal_wroga( i->x + 43, i->y + 3 ) );
				   }
			   }
		   }

		   for( int i = 0; i < obiekty_struktury_strzalu_wroga.size(); i++ )
		   {
                obiekty_struktury_strzalu_wroga[i].przesun_pocisk( wysokosc );
           }

		   for( int i = 0; i < obiekty_struktury_strzalu_wroga.size(); i++)
		   {
			   if( obiekty_struktury_strzalu_wroga[i].poza_oknem() )
			   {
				   obiekty_struktury_strzalu_wroga.erase( obiekty_struktury_strzalu_wroga.begin() + i );
			   }
		   }

		   wrog_spawn += zegar_wroga.restart();
		   if( wrog_spawn >= wrogGranica && !bBoss )
		   {
			   obiekty_struktury_wroga.push_back( wrog_struktura( wylosowana, -60 ) );
               wrog_spawn = Time::Zero;
		   }

		   for( int i = 0; i < obiekty_struktury_wroga.size(); i++ )
		   {
               obiekty_struktury_wroga[i].przesun_wroga( wysokosc );
		   }

		   for( int i = 0; i < obiekty_struktury_wroga.size(); i++)
		   {
			   if( obiekty_struktury_wroga[i].poza_oknem() )
			   {
				   obiekty_struktury_wroga.erase( obiekty_struktury_wroga.begin() + i );
				   zycie_int -= 64;
			   }
		   }

		   boss_spawn += zegar_bossa.restart();

		   if( boss_spawn >= bossGranica && !bBoss )
		   {
			   bBoss = true;
			   bUstawienia = true;

			   boss_spawn = Time::Zero;
		   }

		   if( bBoss )
		   {
			   if( bUstawienia )
			   {
				   boss_str.set_x( wylosowana );
				   boss_str.set_y( -123 );
				   boss_str.set_zycie( 4 );

				   bUstawienia = false;
			   }

			   boss_str.przesun_bossa( statek.getPosition().x - 9, 5 );
		   }

		   for( int i = 0; i < obiekty_struktury_wroga.size(); i++ )
		   {
               for( int j = 0; j < obiekty_struktury_strzalu.size(); j++ )
			   {
				   if( Collision::PixelPerfectTest( obiekty_struktury_wroga[i].zwroc_sprite( wrog ), obiekty_struktury_strzalu[j].zwroc_sprite( pocisk ) ) )
				   {
					   score_int += 10;

					   obiekty_struktury_strzalu.erase( obiekty_struktury_strzalu.begin() + j );

					   obiekty_struktury_wroga[i].zycie -= 1;

					   if( obiekty_struktury_wroga[i].zycie <= 0 )
                       obiekty_struktury_wroga.erase( obiekty_struktury_wroga.begin() + i );
                   }
               }
           }

		   if( bBoss )
		   {
			   for( int i = 0; i < obiekty_struktury_strzalu.size(); i++ )
		       {
				   if( Collision::PixelPerfectTest( boss_str.zwroc_sprite( boss ), obiekty_struktury_strzalu[i].zwroc_sprite( pocisk ) ) )
			        {
						score_int += 16;

						boss_str.zycie -= 1;

						if( boss_str.zycie <= 0 )
						{
							bBoss = false;
							boss_spawn = Time::Zero;
						}

				        obiekty_struktury_strzalu.erase( obiekty_struktury_strzalu.begin() + i );
			        }
		       } 
		   }

		   for( int i = 0; i < obiekty_struktury_strzalu_wroga.size(); i++ )
		   {
			   if( Collision::PixelPerfectTest( statek, obiekty_struktury_strzalu_wroga[i].zwroc_sprite( pocisk_wroga ) ) )
			   {
				   obiekty_struktury_strzalu_wroga.erase( obiekty_struktury_strzalu_wroga.begin() + i );
				   zycie_int -= 32;
			   }
		   }

		   for( int i = 0; i < obiekty_struktury_strzalu_bossa.size(); i++ )
		   {
			   if( Collision::BoundingBoxTest( statek, obiekty_struktury_strzalu_bossa[i].zwroc_sprite( pocisk_bossa ) ) )
			   {
				   obiekty_struktury_strzalu_bossa.erase( obiekty_struktury_strzalu_bossa.begin() + i );
				   zycie_int -= 64;
			   }
		   }

		   for( int i = 0; i < obiekty_struktury_wroga.size(); i++ )
		   {
			   if( Collision::PixelPerfectTest( statek, obiekty_struktury_wroga[i].zwroc_sprite( wrog ) ) )
			   {
				   obiekty_struktury_wroga.erase( obiekty_struktury_wroga.begin() + i );

				   gra = false;
				   koniec = true;
				   gra_okno.close();
			   }
		   }
		   
		   if( zycie_int <= 0 )
		   {
			   gra = false;
			   koniec = true;
			   gra_okno.close();
		   }

		   gra_okno.clear();

           gra_okno.draw( tlo );

		   gra_okno.draw( tekst_score );

		   gra_okno.draw( wyswietl_score );

		   for( int i = 0; i < zycie_int; i += 32 )
		   {
			   zycie.setPosition( szerokosc - 32 - i, 0 );
			   gra_okno.draw( zycie );
		   }

		   for( Obiekty_Struktury_Strzalu::const_iterator i = obiekty_struktury_strzalu.begin(); i != obiekty_struktury_strzalu.end(); i++ )
		   {
			   pocisk.setPosition( i->x, i->y );
			   gra_okno.draw( pocisk );
           }

		   for( Obiekty_Struktury_Strzalu_Wroga::const_iterator i = obiekty_struktury_strzalu_wroga.begin(); i != obiekty_struktury_strzalu_wroga.end(); i++ )
		   {
			   pocisk_wroga.setPosition( i->x, i->y );
			   gra_okno.draw( pocisk_wroga );
           }

		   for( Obiekty_Struktury_Strzalu_Bossa::const_iterator i = obiekty_struktury_strzalu_bossa.begin(); i != obiekty_struktury_strzalu_bossa.end(); i++ )
		   {
			    pocisk_bossa.setPosition( i->x, i->y );
			    gra_okno.draw( pocisk_bossa );
           }

		   if( bBoss )
		   {
			   boss_str.rysuj( gra_okno, boss );
		   }

		   for( Obiekty_Struktury_Wroga::const_iterator i = obiekty_struktury_wroga.begin(); i != obiekty_struktury_wroga.end(); i++ )
		   {
			   wrog.setPosition( i->x, i->y );
			   gra_okno.draw( wrog );
		   }

		   gra_okno.draw( statek );

           gra_okno.display();

		   dt = deltaClock.restart();
		 }
	}

	if( koniec )
	{
		RenderWindow koniec_okno( VideoMode ( szerokosc, wysokosc ), "Space Killer" );

		strzal_wroga_co = 1.07f;
        strzal_bossa_co = 1.07f;
	    strzal_wrogaGranica = seconds( strzal_wroga_co );
	    strzal_bossaGranica = seconds( strzal_bossa_co );

	    for( int i = 0; i < obiekty_struktury_wroga.size(); i++ )
		{
			obiekty_struktury_wroga.erase( obiekty_struktury_wroga.begin() + i );
		}

		bBoss = false;

		for( int i = 0; i < obiekty_struktury_strzalu.size(); i++ )
		{
			obiekty_struktury_strzalu.erase( obiekty_struktury_strzalu.begin() + i );
		}

		for( int i = 0; i < obiekty_struktury_strzalu_wroga.size(); i++ )
		{
			obiekty_struktury_strzalu_wroga.erase( obiekty_struktury_strzalu_wroga.begin() + i );
		}

		for( int i = 0; i < obiekty_struktury_strzalu_bossa.size(); i++ )
		{
			obiekty_struktury_strzalu_bossa.erase( obiekty_struktury_strzalu_bossa.begin() + i );
		}

		Text nasz_score( "Score", czcionka, 60 );
		nasz_score.setStyle( Text::Bold );
		nasz_score.setPosition( szerokosc/2 - nasz_score.getGlobalBounds().width/2, 100);

		string score_koniec;
		Text wyswietl_score_koniec;
		wyswietl_score_koniec.setFont( czcionka );
		wyswietl_score_koniec.setCharacterSize( 50 );
		wyswietl_score_koniec.setStyle( Text::Bold );
		score_koniec = na_string( score_int );
		wyswietl_score_koniec.setString( score_koniec );
		wyswietl_score_koniec.setPosition( szerokosc/2 - wyswietl_score_koniec.getGlobalBounds().width/2, 160 );

		Text play_again( "Play again", czcionka, 50 );
		play_again.setStyle( Text::Bold );
		play_again.setPosition( szerokosc/2 - play_again.getGlobalBounds().width/2, 400);

		while( koniec_okno.isOpen() )
		{
			Event zdarzenie_koniec;
            while( koniec_okno.pollEvent( zdarzenie_koniec ) )
			{
				if( zdarzenie_koniec.type == Event::Closed )
				{
					koniec_okno.close();
					program = false;
				}

			    if ( Mouse::getPosition( koniec_okno ).x > play_again.getGlobalBounds().left &&
                     Mouse::getPosition( koniec_okno ).y > play_again.getGlobalBounds().top &&
                     Mouse::getPosition( koniec_okno ).x < play_again.getGlobalBounds().left + play_again.getGlobalBounds().width &&
                     Mouse::getPosition( koniec_okno ).y < play_again.getGlobalBounds().top +  play_again.getGlobalBounds().height )
				    {
                        play_again.setColor( Color::Cyan );
                    }

				else
					play_again.setColor( Color::White );
			 }

			if ( Mouse::getPosition( koniec_okno ).x > play_again.getGlobalBounds().left &&
                 Mouse::getPosition( koniec_okno ).y > play_again.getGlobalBounds().top &&
                 Mouse::getPosition( koniec_okno ).x < play_again.getGlobalBounds().left + play_again.getGlobalBounds().width &&
                 Mouse::getPosition( koniec_okno ).y < play_again.getGlobalBounds().top +  play_again.getGlobalBounds().height && Mouse::isButtonPressed( Mouse::Left ))
				    {
						gra = true;
                        menu = false;
						koniec = false;
						koniec_okno.close();
                    }

			koniec_okno.clear();

			koniec_okno.draw( tlo );

			koniec_okno.draw( nasz_score );
			koniec_okno.draw( wyswietl_score_koniec );

			koniec_okno.draw( play_again );

			koniec_okno.display();
		}
	}
    
}
	return 0;
}