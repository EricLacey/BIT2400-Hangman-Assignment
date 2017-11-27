// Title: Assignment-5
//Author: Eric Lacey
//Objective : Create a hangman game demonstrating the use of input and output of files
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <conio.h>
#include <string>
#include <vector>
#include <time.h>


using namespace std;

//define a set of gamestates
enum GameState { Menu, Play, HighScore, Exit };
enum PlayState { ChoosePlay, PlayNice, PlayBad, Win, Fail };



//define a cursor object to be used in game (modified from notes)
struct Cursor
{
	int m_X;
	int m_Y;
	int m_PrevX;
	int m_PrevY;
	int m_AsciiChar;
	Cursor() {};
	Cursor(int x, int y, int ascii) : m_X(x), m_Y(y), m_PrevX(x), m_PrevY(y), m_AsciiChar(ascii) {};
};

//function declarations
void gotoXY(int x, int y);
int selector(vector<COORD> positions);
void drawScreen(int attemptNum, vector<char> usedLetters);
void drawBody(int attemptNum, COORD startpoint);
void deathAni(COORD startpoint);

int main()
{

	//declare a gamestate for the game
	GameState state = Menu;

	while (true)
	{
		if (state == Menu)
		{
			gotoXY(0, 0);

			cout << "Hangman!\n\n";
			cout << "  - New Game\n";
			cout << "  - High Scores\n";
			cout << "  - Exit\n";


			vector<COORD> menuLocations = { { 1,2 } ,{ 1,3 } ,{ 1,4 } };


			int test = selector(menuLocations);

			if (test == 0)
			{
				state = Play;
			}
			else if (test == 1)
			{
				state = HighScore;
			}
			else if (test == 2)
			{
				state = Exit;
			}

		}

		else if (state == Play)
		{
			//initialize a playstate
			PlayState playstate = ChoosePlay;

			while (true)
			{
				//gameplay menu
				if (playstate == ChoosePlay)
				{
					system("cls");
					gotoXY(0, 0);

					cout << "Are you...\n\n";
					cout << "  -going to play nice?\n";
					cout << "           or\n";
					cout << "  -going to play naughty?\n\n";
					cout << "  -return to main menu";

					//set locations for each menu point
					vector<COORD> menuLocations = { { 1,2 } ,{ 1,4 } ,{ 1,6 } };

					//choose a location
					int test = selector(menuLocations);

					//switch state from menu hit
					if (test == 0)
					{
						playstate = PlayNice;
					}
					else if (test == 1)
					{
						playstate = PlayBad;
					}
					else if (test == 2)
					{
						state = Menu;
						break;
					}

					system("cls");


				}
				else if (playstate == PlayNice)
				{
					//create a vector to hold the position of each line
					vector<int> lineLoc;

					//open ifstream for dictionary to be used
					ifstream dictionaryInput;
					dictionaryInput.open("10000CommonMediumWords.txt");

					//test to make sure file is open
					if (!dictionaryInput.is_open())
					{
						cout << "Error, dictionary unable to open" << endl;
						system("pause");
						return 1;
					}

					//record every line's location, word count will be lineLoc.size
					while (!(dictionaryInput.eof()))
					{
						lineLoc.push_back(dictionaryInput.tellg());
						string test;
						getline(dictionaryInput, test);
					}

					//set random seed and go to random word's line
					srand(time(0));
					dictionaryInput.seekg(lineLoc[rand() % lineLoc.size()]);

					//select word from file
					string hangmanWord;
					getline(dictionaryInput, hangmanWord);

					//variables to be used in game
					vector<char> readout;
					vector<char> usedLetters;
					int pressedLetter;
					int attemptNum = 1;
					bool validLetter;
					bool correctLetter;

					for (int i = 0; i < hangmanWord.size(); i++)
					{

						if (hangmanWord[i] == ' ' || hangmanWord[i] == '&' || isdigit(hangmanWord[i]))
						{
							readout.push_back(hangmanWord[i]);
						}
						else
						{
							readout.push_back('_');
						}

					}

					//draw gameplay area
					drawScreen(attemptNum, usedLetters);
					gotoXY(8, 7);

					for (int i = 0; i < readout.size(); i++)
					{
						cout << readout[i] << " ";
					}

					//game is actually played
					while (true)
					{
						//setup for keypress testing
						validLetter = true;
						correctLetter = false;

						//read keypress and test
						if (_kbhit())
						{
							pressedLetter = _getch();

							//make sure pressed key is a letter
							if (isalpha((char)pressedLetter))
							{
								//check to see if letter has already been pressed
								for (int i = 0; i < usedLetters.size(); i++)
								{
									if ((char)pressedLetter == usedLetters[i])
									{
										validLetter = false;
										break;
									}
								}

								//if good, use letter
								if (validLetter == true)
								{
									for (int i = 0; i < hangmanWord.size(); i++)
									{
										if ((char)pressedLetter == hangmanWord[i])
										{
											correctLetter = true;
											readout[i] = hangmanWord[i];
										}
									}

									if (correctLetter == false)
									{
										attemptNum++;
									}

									//add letter to usedLetters
									usedLetters.push_back((char)pressedLetter);
								}
							}
							
							//redraw gameplay area
							drawScreen(attemptNum, usedLetters);
							gotoXY(8, 7);

							for (int i = 0; i < readout.size(); i++)
							{
								cout << readout[i] << " ";
							}

						}

						//check if wincon is set
						bool wincon = true;
						for (int i = 0; i < hangmanWord.size(); i++)
						{
							if (readout[i] != hangmanWord[i])
							{
								wincon = false;
							}
						}

						if (wincon == true)
						{
							ofstream addWinner;
							addWinner.open("winner.txt", ofstream::out | ofstream::app);
							if (!addWinner.is_open())
							{
								cout << "Error, winner list unable to open" << endl;
								system("pause");
								return 1;
							}

							system("cls");
							gotoXY(0, 0);

							string winnerName;
							cout << "You won! Please give your name to add to the winners list: " << endl;
							cin >> winnerName;

							addWinner << winnerName << " - " << hangmanWord << '\n'; 

							addWinner.close();

							playstate = ChoosePlay;
							system("cls");
							break;
						}

						//test for failcon
						if (attemptNum > 8)
						{
							system("cls");
							gotoXY(0, 0);

							cout << "You Lose! He had a wife and family you monster!" << endl;

							system("pause");
							playstate = ChoosePlay;
							system("cls");
							break;
						}
					}


					///////////////////////////
					//close dictionary
					dictionaryInput.close();
				}
				else if (playstate == PlayBad)
				{
					//create a vector to hold the position of each line
					vector<int> lineLoc;

					//open ifstream for dictionary to be used
					ifstream badDictionaryInput;
					badDictionaryInput.open("List-of-Dirty-Naughty-Obscene-and-Otherwise-Bad-Words.txt");
								
					//test to make sure file is open
					if (!badDictionaryInput.is_open())
					{
						cout << "Error, dictionary unable to open" << endl;
						system("pause");
						return 1;
					}

					//record every line's location, word count will be lineLoc.size
					while (!(badDictionaryInput.eof()))
					{
						lineLoc.push_back(badDictionaryInput.tellg());
						string test;
						getline(badDictionaryInput, test);
					}

					//set random seed and go to random word's line
					srand(time(0));
					badDictionaryInput.seekg(lineLoc[rand() % lineLoc.size()]);

					//select word from file
					string hangmanWord;
					getline(badDictionaryInput, hangmanWord);

					//variables to be used in game
					vector<char> readout;
					vector<char> usedLetters;
					int pressedLetter;
					int attemptNum = 1;
					bool validLetter;
					bool correctLetter;

					for (int i = 0; i < hangmanWord.size(); i++)
					{

						if (hangmanWord[i] == ' ' || hangmanWord[i] == '&' || isdigit(hangmanWord[i]))
						{
							readout.push_back(hangmanWord[i]);
						}
						else
						{
							readout.push_back('_');
						}

					}

					//draw gameplay area
					drawScreen(attemptNum, usedLetters);
					gotoXY(8, 7);

					for (int i = 0; i < readout.size(); i++)
					{
						cout << readout[i] << " ";
					}

					//game is actually played
					while (true)
					{
						//setup for keypress testing
						validLetter = true;
						correctLetter = false;

						//read keypress and test
						if (_kbhit())
						{
							pressedLetter = _getch();

							//make sure pressed key is a letter
							if (isalpha((char)pressedLetter))
							{
								//check to see if letter has already been pressed
								for (int i = 0; i < usedLetters.size(); i++)
								{
									if ((char)pressedLetter == usedLetters[i])
									{
										validLetter = false;
										break;
									}
								}

								//if good, use letter
								if (validLetter == true)
								{
									for (int i = 0; i < hangmanWord.size(); i++)
									{
										if ((char)pressedLetter == hangmanWord[i])
										{
											correctLetter = true;
											readout[i] = hangmanWord[i];
										}
									}

									if (correctLetter == false)
									{
										attemptNum++;
									}

									//add letter to usedLetters
									usedLetters.push_back((char)pressedLetter);
								}
							}

							//redraw gameplay area
							drawScreen(attemptNum, usedLetters);
							gotoXY(8, 7);

							for (int i = 0; i < readout.size(); i++)
							{
								cout << readout[i] << " ";
							}

						}

						//check if wincon is set
						bool wincon = true;
						for (int i = 0; i < hangmanWord.size(); i++)
						{
							if (readout[i] != hangmanWord[i])
							{
								wincon = false;
							}
						}

						if (wincon == true)
						{
							ofstream addWinner;
							addWinner.open("winner.txt", ofstream::out | ofstream::app);
							if (!addWinner.is_open())
							{
								cout << "Error, winner list unable to open" << endl;
								system("pause");
								return 1;
							}

							system("cls");
							gotoXY(0, 0);

							string winnerName;
							cout << "You won you raunchy fuck! Please give your name to add to the winners list like the glorious bastard you are: " << endl;
							cin >> winnerName;

							addWinner << winnerName << " - " << hangmanWord << '\n';

							playstate = ChoosePlay;
							system("cls");
							break;
						}

						//test for failcon
						if (attemptNum > 8)
						{
							system("cls");
							gotoXY(0, 0);

							cout << "You fucking lost! He had a wife and family you murderous piece of shit!" << endl;

							system("pause");
							playstate = ChoosePlay;
							system("cls");
							break;
						}
					}

					badDictionaryInput.close();
				}


			}
			system("cls");
		}

		else if (state == HighScore)
		{
			system("cls");
			gotoXY(0, 0);

			//open file instream for loading
			ifstream HighScoreIn;
			HighScoreIn.open("winner.txt");

			//checking if file is open
			if (!HighScoreIn.is_open())
			{
				cout << "Error, winner list unable to open" << endl;
				system("pause");
				return 1;
			}

			//read all the high scores
			while (!HighScoreIn.eof())
			{
				string readline;
				getline(HighScoreIn, readline);
				cout << readline << endl;
			}

			HighScoreIn.close();
			system("pause");
			
		}

		else if (state == Exit)
		{

			return 0;
		}
	}

}


//goto position function from notes
void gotoXY(int x, int y)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { x, y };
	SetConsoleCursorPosition(hStdout, position);
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.bVisible = false;
	cursorInfo.dwSize = 10;
SetConsoleCursorInfo(hStdout, &cursorInfo);

}

//function taken from notes and modified to fit menu application
//function to select a listed option using a cursor on screen
int selector(vector<COORD> positions)
{
	//set an index for future reference
	int i = 0;

	//define a cursor to be used as the selector
	Cursor cursor = Cursor(positions[0].X, positions[0].Y, '>');

	gotoXY(cursor.m_X, cursor.m_Y);
	cout << (char)cursor.m_AsciiChar;

	//create an int to be used to store the character of kepresses
	int character;

	while (true)
	{
		if (_kbhit())
		{
			//get keypress
			character = _getch();

			//action basaed on keypress
			if (character == 80 && i < positions.size() - 1) //down and not at bottom
			{
				//move to next location
				i++;
				cursor.m_X = positions[i].X;
				cursor.m_Y = positions[i].Y;
			}
			else if (character == 72 && i > 0) //up and not at top
			{
				//move to previous location
				i--;
				cursor.m_X = positions[i].X;
				cursor.m_Y = positions[i].Y;
			}
			else if (character == 13)
			{
				//select location you are at
				return i;
			}

			//go to new location
			gotoXY(cursor.m_X, cursor.m_Y);
			cout << (char)cursor.m_AsciiChar;

			//if the character moved, update its new location and clear its old location
			if ((cursor.m_X != cursor.m_PrevX) || (cursor.m_Y != cursor.m_PrevY))
			{
				gotoXY(cursor.m_PrevX, cursor.m_PrevY);
				cout << ' ';
				cursor.m_PrevX = cursor.m_X;
				cursor.m_PrevY = cursor.m_Y;
			}
		}
	}
}



void drawScreen(int attemptNum, vector<char> usedLetters)
{

	//draw the body
	COORD bodyDrawPoint = { 37, 2 };
	if (attemptNum <= 8)
	{
		drawBody(attemptNum, bodyDrawPoint);
	}
	else
	{
		drawBody(attemptNum, bodyDrawPoint);
		deathAni(bodyDrawPoint);
	}

	gotoXY(4, 14);
	cout << "       used letters      ";
	gotoXY(4, 15);
	cout << " ------------------------";
	gotoXY(4, 16);
	cout << "|                        |";
	gotoXY(4, 17);
	cout << "|                        |";
	gotoXY(4, 18);
	cout << " ------------------------";

	gotoXY(5, 16);
	for (int i = 0; i < usedLetters.size(); i++)
	{
		if (i == 12)
		{
			gotoXY(5, 17);
		}

		cout << ' ' << usedLetters[i];
		
	}
}

void drawBody(int attemptNum, COORD startpoint)
{
	//draw stand
	if (attemptNum > 1)
	{
		gotoXY(startpoint.X + 4, startpoint.Y + 0);
		cout << (char)201 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)187;
		gotoXY(startpoint.X + 15, startpoint.Y + 1);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 2);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 3);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 4);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 5);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 6);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 7);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 8);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 9);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 10);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 11);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 12);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 13);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 14);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 15);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 16);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 17);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 18);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 19);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 20);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 21);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 22);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 23);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 24);
		cout << (char)186;
		gotoXY(startpoint.X + 15, startpoint.Y + 25);
		cout << (char)186;
		gotoXY(startpoint.X + 10, startpoint.Y + 26);
		cout << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)202 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205;
	}
	//draw head
	if (attemptNum > 2)
	{
		gotoXY(startpoint.X + 1, startpoint.Y + 2);
		cout << (char)218 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)191;
		gotoXY(startpoint.X + 1, startpoint.Y + 3);
		cout << (char)179 << " O O " << (char)179;
		gotoXY(startpoint.X + 1, startpoint.Y + 4);
		cout << (char)179 << "     " << (char)179;
		gotoXY(startpoint.X + 1, startpoint.Y + 5);
		cout << (char)179 << "     " << (char)179;
		gotoXY(startpoint.X + 1, startpoint.Y + 6);
		cout << (char)192 << (char)196 << (char)196 << (char)194 << (char)196 << (char)196 << (char)217;

	}
	//draw torso
	if (attemptNum > 3)
	{
		gotoXY(startpoint.X + 4, startpoint.Y + 7);
		cout << (char)179;
		gotoXY(startpoint.X + 3, startpoint.Y + 8);
		cout << (char)196 << (char)197 << (char)196;
		gotoXY(startpoint.X + 4, startpoint.Y + 9);
		cout << (char)179;
		gotoXY(startpoint.X + 4, startpoint.Y + 10);
		cout << (char)179;
		gotoXY(startpoint.X + 4, startpoint.Y + 11);
		cout << (char)179;
		gotoXY(startpoint.X + 4, startpoint.Y + 12);
		cout << (char)179;
		gotoXY(startpoint.X + 4, startpoint.Y + 13);
		cout << (char)179;
		gotoXY(startpoint.X + 4, startpoint.Y + 14);
		cout << (char)193;
	}
	//draw left arm
	if (attemptNum > 4)
	{
		gotoXY(startpoint.X + 0, startpoint.Y + 8);
		cout << (char)218 << (char)196 << (char)196 << (char)196;
		gotoXY(startpoint.X + 0, startpoint.Y + 9);
		cout << (char)179;
		gotoXY(startpoint.X + 0, startpoint.Y + 10);
		cout << (char)179;
		gotoXY(startpoint.X + 0, startpoint.Y + 11);
		cout << (char)179;
		gotoXY(startpoint.X + 0, startpoint.Y + 12);
		cout << (char)179;
	}
	//draw right arm
	if (attemptNum > 5)
	{
		gotoXY(startpoint.X + 5, startpoint.Y + 8);
		cout << (char)196 << (char)196 << (char)196 << (char)191;
		gotoXY(startpoint.X + 8, startpoint.Y + 9);
		cout << (char)179;
		gotoXY(startpoint.X + 8, startpoint.Y + 10);
		cout << (char)179;
		gotoXY(startpoint.X + 8, startpoint.Y + 11);
		cout << (char)179;
		gotoXY(startpoint.X + 8, startpoint.Y + 12);
		cout << (char)179;
	}
	//draw left leg
	if (attemptNum > 6)
	{
		gotoXY(startpoint.X + 3, startpoint.Y + 14);
		cout << (char)47;
		gotoXY(startpoint.X + 2, startpoint.Y + 15);
		cout << (char)47;
		gotoXY(startpoint.X + 2, startpoint.Y + 16);
		cout << (char)179;
		gotoXY(startpoint.X + 2, startpoint.Y + 17);
		cout << (char)179;
		gotoXY(startpoint.X + 2, startpoint.Y + 18);
		cout << (char)179;
		gotoXY(startpoint.X + 2, startpoint.Y + 19);
		cout << (char)179;
		gotoXY(startpoint.X + 2, startpoint.Y + 20);
		cout << (char)179;
		gotoXY(startpoint.X + 2, startpoint.Y + 21);
		cout << (char)179;
	}
	//draw right leg
	if (attemptNum > 7)
	{
		gotoXY(startpoint.X + 5, startpoint.Y + 14);
		cout << (char)92;
		gotoXY(startpoint.X + 6, startpoint.Y + 15);
		cout << (char)92;
		gotoXY(startpoint.X + 6, startpoint.Y + 16);
		cout << (char)179;
		gotoXY(startpoint.X + 6, startpoint.Y + 17);
		cout << (char)179;
		gotoXY(startpoint.X + 6, startpoint.Y + 18);
		cout << (char)179;
		gotoXY(startpoint.X + 6, startpoint.Y + 19);
		cout << (char)179;
		gotoXY(startpoint.X + 6, startpoint.Y + 20);
		cout << (char)179;
		gotoXY(startpoint.X + 6, startpoint.Y + 21);
		cout << (char)179;
	}
	//draw noose + trap
	if (attemptNum > 8)
	{
		//noose
		gotoXY(startpoint.X + 4, startpoint.Y + 1);
		cout << (char)179;
		//stand
		gotoXY(startpoint.X + 0, startpoint.Y + 22);
		cout << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205;
	}
}

void deathAni(COORD startpoint)
{
	int animationFrame = 0;
	int stationaryItemY = startpoint.Y;

	//draw fall
	for (int i = 0; i < 6; i++)
	{
		//erase old frame
		for (int j = 1; j <= 26; j++)
		{
			gotoXY(startpoint.X + 0, startpoint.Y + j);
			cout << "         ";
		}

		//draw new frame

		//noose
		for (int i = 0; i <= startpoint.Y - stationaryItemY; i++)
		{
			gotoXY(startpoint.X + 4, stationaryItemY + 1 + i);
			cout << (char)179;
		}

		//trap
		gotoXY(startpoint.X + 0, stationaryItemY + 22);
		cout << (char)201 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)187;
		gotoXY(startpoint.X + 0, stationaryItemY + 23);
		cout << (char)186 << "       " << (char)186;
		gotoXY(startpoint.X + 0, stationaryItemY + 24);
		cout << (char)186 << "       " << (char)186;
		gotoXY(startpoint.X + 0, stationaryItemY + 25);
		cout << (char)200 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)188;

		//draw head
		gotoXY(startpoint.X + 1, startpoint.Y + 2);
		cout << (char)218 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)191;
		gotoXY(startpoint.X + 1, startpoint.Y + 3);
		cout << (char)179 << " O O " << (char)179;
		gotoXY(startpoint.X + 1, startpoint.Y + 4);
		cout << (char)179 << "     " << (char)179;
		gotoXY(startpoint.X + 1, startpoint.Y + 5);
		cout << (char)179 << "     " << (char)179;
		gotoXY(startpoint.X + 1, startpoint.Y + 6);
		cout << (char)192 << (char)196 << (char)196 << (char)194 << (char)196 << (char)196 << (char)217;

		//draw torso

		gotoXY(startpoint.X + 4, startpoint.Y + 7);
		cout << (char)179;
		gotoXY(startpoint.X + 3, startpoint.Y + 8);
		cout << (char)196 << (char)197 << (char)196;
		gotoXY(startpoint.X + 4, startpoint.Y + 9);
		cout << (char)179;
		gotoXY(startpoint.X + 4, startpoint.Y + 10);
		cout << (char)179;
		gotoXY(startpoint.X + 4, startpoint.Y + 11);
		cout << (char)179;
		gotoXY(startpoint.X + 4, startpoint.Y + 12);
		cout << (char)179;
		gotoXY(startpoint.X + 4, startpoint.Y + 13);
		cout << (char)179;
		gotoXY(startpoint.X + 4, startpoint.Y + 14);
		cout << (char)193;
		//draw left arm
		gotoXY(startpoint.X + 0, startpoint.Y + 8);
		cout << (char)218 << (char)196 << (char)196 << (char)196;
		gotoXY(startpoint.X + 0, startpoint.Y + 9);
		cout << (char)179;
		gotoXY(startpoint.X + 0, startpoint.Y + 10);
		cout << (char)179;
		gotoXY(startpoint.X + 0, startpoint.Y + 11);
		cout << (char)179;
		gotoXY(startpoint.X + 0, startpoint.Y + 12);
		cout << (char)179;
		//draw right arm
		gotoXY(startpoint.X + 5, startpoint.Y + 8);
		cout << (char)196 << (char)196 << (char)196 << (char)191;
		gotoXY(startpoint.X + 8, startpoint.Y + 9);
		cout << (char)179;
		gotoXY(startpoint.X + 8, startpoint.Y + 10);
		cout << (char)179;
		gotoXY(startpoint.X + 8, startpoint.Y + 11);
		cout << (char)179;
		gotoXY(startpoint.X + 8, startpoint.Y + 12);
		cout << (char)179;
		//draw left leg
		gotoXY(startpoint.X + 3, startpoint.Y + 14);
		cout << (char)47;
		gotoXY(startpoint.X + 2, startpoint.Y + 15);
		cout << (char)47;
		gotoXY(startpoint.X + 2, startpoint.Y + 16);
		cout << (char)179;
		gotoXY(startpoint.X + 2, startpoint.Y + 17);
		cout << (char)179;
		gotoXY(startpoint.X + 2, startpoint.Y + 18);
		cout << (char)179;
		gotoXY(startpoint.X + 2, startpoint.Y + 19);
		cout << (char)179;
		gotoXY(startpoint.X + 2, startpoint.Y + 20);
		cout << (char)179;
		gotoXY(startpoint.X + 2, startpoint.Y + 21);
		cout << (char)179;
		//draw right leg
		gotoXY(startpoint.X + 5, startpoint.Y + 14);
		cout << (char)92;
		gotoXY(startpoint.X + 6, startpoint.Y + 15);
		cout << (char)92;
		gotoXY(startpoint.X + 6, startpoint.Y + 16);
		cout << (char)179;
		gotoXY(startpoint.X + 6, startpoint.Y + 17);
		cout << (char)179;
		gotoXY(startpoint.X + 6, startpoint.Y + 18);
		cout << (char)179;
		gotoXY(startpoint.X + 6, startpoint.Y + 19);
		cout << (char)179;
		gotoXY(startpoint.X + 6, startpoint.Y + 20);
		cout << (char)179;
		gotoXY(startpoint.X + 6, startpoint.Y + 21);
		cout << (char)179;

		startpoint.Y++;
		Sleep(200);
	}

	//draw swinging

	// have positions -1,0,1
	int last_i = -1;
	int i = 0;

	int currentTime = GetTickCount();

	while (true)
	{
		if (GetTickCount() - currentTime > 1000)
		{
			//erase old frame
			for (int j = 1; j <= 27; j++)
			{
				gotoXY(startpoint.X - 1, stationaryItemY + j);
				cout << "           ";
			}

			//draw stationary objects
			{
				//trap
				gotoXY(startpoint.X + 0, stationaryItemY + 22);
				cout << (char)201 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)187;
				gotoXY(startpoint.X + 0, stationaryItemY + 23);
				cout << (char)186 << "       " << (char)186;
				gotoXY(startpoint.X + 0, stationaryItemY + 24);
				cout << (char)186 << "       " << (char)186;
				gotoXY(startpoint.X + 0, stationaryItemY + 25);
				cout << (char)200 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)205 << (char)188;
			}

			//draw noose
			if (i == 1)
			{
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 1);
				cout << "\\";
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 2);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 3);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 4);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 5);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 6);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 7);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 8);
				cout << (char)179;
			}
			else if (i == 0)
			{
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 1);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 2);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 3);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 4);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 5);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 6);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 7);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 8);
				cout << (char)179;
			}
			else if (i == -1)
			{
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 1);
				cout << "/";
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 2);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 3);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 4);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 5);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 6);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 7);
				cout << (char)179;
				gotoXY(startpoint.X + 4 + i, stationaryItemY + 8);
				cout << (char)179;
			}

			//draw head
			gotoXY(startpoint.X + 1 + i, startpoint.Y + 2);
			cout << (char)218 << (char)196 << (char)196 << (char)196 << (char)196 << (char)196 << (char)191;
			gotoXY(startpoint.X + 1 + i, startpoint.Y + 3);
			cout << (char)179 << " - - " << (char)179;
			gotoXY(startpoint.X + 1 + i, startpoint.Y + 4);
			cout << (char)179 << "     " << (char)179;
			gotoXY(startpoint.X + 1 + i, startpoint.Y + 5);
			cout << (char)179 << "  @  " << (char)179;
			gotoXY(startpoint.X + 1 + i, startpoint.Y + 6);
			cout << (char)192 << (char)196 << (char)196 << (char)194 << (char)196 << (char)196 << (char)217;

			//draw torso

			gotoXY(startpoint.X + 4 + i, startpoint.Y + 7);
			cout << (char)179;
			gotoXY(startpoint.X + 3 + i, startpoint.Y + 8);
			cout << (char)196 << (char)197 << (char)196;
			gotoXY(startpoint.X + 4 + i, startpoint.Y + 9);
			cout << (char)179;
			gotoXY(startpoint.X + 4 + i, startpoint.Y + 10);
			cout << (char)179;
			gotoXY(startpoint.X + 4 + i, startpoint.Y + 11);
			cout << (char)179;
			gotoXY(startpoint.X + 4 + i, startpoint.Y + 12);
			cout << (char)179;
			gotoXY(startpoint.X + 4 + i, startpoint.Y + 13);
			cout << (char)179;
			gotoXY(startpoint.X + 4 + i, startpoint.Y + 14);
			cout << (char)193;
			//draw left arm
			gotoXY(startpoint.X + 0 + i, startpoint.Y + 8);
			cout << (char)218 << (char)196 << (char)196 << (char)196;
			gotoXY(startpoint.X + 0 + i, startpoint.Y + 9);
			cout << (char)179;
			gotoXY(startpoint.X + 0 + i, startpoint.Y + 10);
			cout << (char)179;
			gotoXY(startpoint.X + 0 + i, startpoint.Y + 11);
			cout << (char)179;
			gotoXY(startpoint.X + 0 + i, startpoint.Y + 12);
			cout << (char)179;
			//draw right arm
			gotoXY(startpoint.X + 5 + i, startpoint.Y + 8);
			cout << (char)196 << (char)196 << (char)196 << (char)191;
			gotoXY(startpoint.X + 8 + i, startpoint.Y + 9);
			cout << (char)179;
			gotoXY(startpoint.X + 8 + i, startpoint.Y + 10);
			cout << (char)179;
			gotoXY(startpoint.X + 8 + i, startpoint.Y + 11);
			cout << (char)179;
			gotoXY(startpoint.X + 8 + i, startpoint.Y + 12);
			cout << (char)179;
			//draw left leg
			gotoXY(startpoint.X + 3 + i, startpoint.Y + 14);
			cout << (char)47;
			gotoXY(startpoint.X + 2 + i, startpoint.Y + 15);
			cout << (char)47;
			gotoXY(startpoint.X + 2 + i, startpoint.Y + 16);
			cout << (char)179;
			gotoXY(startpoint.X + 2 + i, startpoint.Y + 17);
			cout << (char)179;
			gotoXY(startpoint.X + 2 + i, startpoint.Y + 18);
			cout << (char)179;
			gotoXY(startpoint.X + 2 + i, startpoint.Y + 19);
			cout << (char)179;
			gotoXY(startpoint.X + 2 + i, startpoint.Y + 20);
			cout << (char)179;
			gotoXY(startpoint.X + 2 + i, startpoint.Y + 21);
			cout << (char)179;
			//draw right leg
			gotoXY(startpoint.X + 5 + i, startpoint.Y + 14);
			cout << (char)92;
			gotoXY(startpoint.X + 6 + i, startpoint.Y + 15);
			cout << (char)92;
			gotoXY(startpoint.X + 6 + i, startpoint.Y + 16);
			cout << (char)179;
			gotoXY(startpoint.X + 6 + i, startpoint.Y + 17);
			cout << (char)179;
			gotoXY(startpoint.X + 6 + i, startpoint.Y + 18);
			cout << (char)179;
			gotoXY(startpoint.X + 6 + i, startpoint.Y + 19);
			cout << (char)179;
			gotoXY(startpoint.X + 6 + i, startpoint.Y + 20);
			cout << (char)179;
			gotoXY(startpoint.X + 6 + i, startpoint.Y + 21);
			cout << (char)179;

			if (i == 1)
			{
				last_i = i;
				i = 0;

			}
			else if (i == -1)
			{
				last_i = i;
				i = 0;
			}
			else if (i - last_i > 0)
			{
				last_i = i;
				i++;
			}
			else if (i - last_i < 0)
			{
				last_i = i;
				i--;
			}

			currentTime = GetTickCount();
		}

		if (_kbhit())
		{
			break;
		}
	}

}