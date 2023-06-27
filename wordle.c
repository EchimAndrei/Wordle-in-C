#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <ctype.h>

#define INALTIMEF 25
#define LATIMEF 40
#define INCERCARI 6
#define LITERE 5

// 6 incercari (randuri)
// 5 litere (coloane)

int ValoareRandom(int max, int min)
{

  // Alegem o valoare random

  int Valoare;
  int interval = max - min + 1;

  srand(time(0));

  Valoare = rand() % interval + min;

  return Valoare;
}

void CreareTABEL(WINDOW *win, int startx, int starty)
{

  // Desenem tabelul
  box(win, 0, 0);
  float distcoloane = LATIMEF / LITERE;
  float distranduri = INALTIMEF / INCERCARI;
  float Adistcoloane = distcoloane;
  float Adistranduri = distranduri;

  // Desenam coloanele
  for (int i = 1; i < INALTIMEF - 1; i++)
  {
    for (int j = 0; j < LITERE - 1; j++)
    {
      mvwaddch(win, i, Adistcoloane, '|');
      Adistcoloane += distcoloane;
    }
    Adistcoloane = distcoloane;
  }

  // Desenam randurile
  for (int i = 1; i < LATIMEF - 1; i++)
  {
    for (int j = 0; j < INCERCARI - 1; j++)
    {
      mvwaddch(win, Adistranduri, i, '-');
      Adistranduri += distranduri;
    }
    Adistranduri = distranduri;
  }
  wrefresh(win);
}

int VerificareCuvant(char cuvant[], char raspuns[], WINDOW *win, float mijcelulay, float mijcelulax)
{

  int castigat = 1;
  int j, i;

  float amijcelulay = mijcelulay, amijcelulax = mijcelulax;

  for (i = 0; i < LITERE; i++)
  {
    int gasit = 0, super_gasit = 0;
    for (j = 0; j < LITERE; j++)
    {
      if ((cuvant[i] == raspuns[j]) && (j == i))
      {

        // Am gasit caracterul in cuvant si este pe pozitia corecta
        super_gasit = 1;
        break;
      }
      if ((cuvant[i] == raspuns[j]) && (j != i))
      {
        // Am gasit caracterul in cuvant, dar nu este pe pozitia corecta
        gasit++;
      }
    }
    amijcelulax = mijcelulax;

    for (int k = 0; k < LITERE; k++)
    {
      if (mvwinch(win, amijcelulay, amijcelulax) == cuvant[i])
      {
        if (super_gasit)
        {
          // Daca am gasit caracterul in cuvant si este pe pozitia corecta il coloram cu verde
          wattron(win, COLOR_PAIR(2));
          mvwaddch(win, amijcelulay, amijcelulax, cuvant[i]);
          wattroff(win, COLOR_PAIR(2));
          break;
        }
        else if (gasit)
        {
          // Daca am gasit caracterul in cuvant, dar nu este pe pozitia corecta il coloram cu galben
          castigat = 0;
          wattron(win, COLOR_PAIR(1));
          mvwaddch(win, amijcelulay, amijcelulax, cuvant[i]);
          wattroff(win, COLOR_PAIR(1));
          break;
        }
        if (super_gasit == 0 && gasit == 0)
        {
          // Daca nu am gasit caracterul in cuvant il coloram cu gri
          castigat = 0;
          wattron(win, COLOR_PAIR(3));
          mvwaddch(win, amijcelulay, amijcelulax, cuvant[i]);
          wattroff(win, COLOR_PAIR(3));
          break;
        }
      }
      else if (mvwinch(win, amijcelulay, amijcelulax) != cuvant[i])
      {
        amijcelulax += mijcelulax * 2;
      }
    }
  }
  wrefresh(win);
  refresh();
  if (castigat == 1)
  {
    return 1;
  }
}

int CitireCeule(WINDOW *win, float mijcelulay, float mijcelulax, char raspuns[], float START_XF)
{

  wrefresh(win);

  char cuvant[6];
  float amijcelulay = mijcelulay, amijcelulax = 3 * mijcelulax;

  int Epuizat = 0;

  // Initializam perechile de culori
  start_color();
  use_default_colors();
  init_pair(1, COLOR_WHITE, COLOR_YELLOW); // Gasit, pozitia gresita
  init_pair(2, COLOR_WHITE, COLOR_GREEN);  // Super gasit
  init_pair(3, COLOR_WHITE, COLOR_BLACK);

  float START_YCI = 5 + INALTIMEF + 2;
  float START_XCI = START_XF;
  WINDOW *CuvantIncomplet = newwin(mijcelulay * 4, LATIMEF, START_YCI, START_XCI);

  while (Epuizat != INCERCARI)
  {
    // Acest while va merge pana cand ramanem fara incercari
    int i = 0;
    char c;
    while (i < LITERE)
    {
    nimic:;
      // Acest while merge pana cand utilizatorul ajunge la penultima litera din fiecare rand
      int ApasatBackspace = 0;
      noecho();
      c = wgetch(win);
      // Introducem de la tastatura un caracter
      if ((int)c == 10)
      {
        // Daca utilizatorul a apasat tasta "Enter" afisam casuta cu "CUVANT INCOMPLET"
        wmove(win, amijcelulay, amijcelulax - 2 * mijcelulax);
        box(CuvantIncomplet, 0, 0);
        mvwprintw(CuvantIncomplet, mijcelulay * 1.5, LATIMEF / 3.3, "CUVANT INCOMPLET!");
        mvwprintw(CuvantIncomplet, mijcelulay * 1.5 + 1, LATIMEF / 3.3 - 7, "Introdu un cuvant de 5 litere!");
        wrefresh(CuvantIncomplet);
        refresh();
      }
      if (c == ':')
      {
        // Daca utilizatorul a apasaut ":" vom returna 2, simbolizand dorinta acestuia de a deschide meniul
      menu:
        return 2;
      }
      if ((int)c == 127 && amijcelulax >= mijcelulax)
      {
        // Daca utilizatorul a apasat Backspace, ne intoarcem la o pozitie anterioara si inlocuim acel caracter cu ' '

        if (i == 0)
          // Daca suntem in prima casuta dintr-un rand, programul nu face nimic
          goto nimic;

      Backspace:
        amijcelulax -= 2 * mijcelulax;
        mvwaddch(win, amijcelulay, amijcelulax - 2 * mijcelulax, ' ');
        wmove(win, amijcelulay, amijcelulax - 2 * mijcelulax);
        i--;
        ApasatBackspace = 1;
      }
      if (isalpha(c) && c != ':' && ApasatBackspace == 0 && amijcelulax >= mijcelulax)
      {
        // Daca utilizatorul a apasat un caracter, vom sterge fereastra "Cuvant Incomplet" (daca aceasta exista), afisam caracterul si in urmatoarea casuta
        werase(CuvantIncomplet);
        wrefresh(CuvantIncomplet);
        echo();
        waddch(win, c);
        wmove(win, amijcelulay, amijcelulax);
        amijcelulax += 2 * mijcelulax;
        cuvant[i] = c;
        i++;
      }
    }

    noecho();
    char ch = wgetch(win);
    while ((int)ch != 10)
    {
      // Cand programul ajunge in acest while inseamna ca utilizatorul este in ultima casuta dintr-un rand si asteapta ca utilizatorul sa apese enter, backspace sau ":"
      if (ch == ':')
        goto menu;
      if ((int)ch == 127)
      {
        goto Backspace;
      }
      ch = wgetch(win);
    }
    echo();

    // Acum verificam cuvantul, iar daca acesta este corect functia va returna 1, simbolizand faptul ca a fost gasit cuvantul
    if (VerificareCuvant(cuvant, raspuns, win, amijcelulay, mijcelulax) == 1)
      return 1;

    // Trecem pe urmatorul rand
    amijcelulay += 2 * mijcelulay;
    amijcelulax = mijcelulax;
    wmove(win, amijcelulay, amijcelulax);
    amijcelulax += 2 * mijcelulax;

    Epuizat++;
  }
  if (Epuizat == INCERCARI)
    // Daca nu mai sunt incercari, inseamna ca utilizatorul a pierdut si functia returneaza 0
    return 0;
}

int MeniuCastig(float mijcelulay, float START_YMC, float START_XMC)
{
  // Cream fereastra de castig
  WINDOW *MeniuCastig = newwin(mijcelulay * 4, LATIMEF, START_YMC, START_XMC);
  wrefresh(MeniuCastig);
  refresh();
  box(MeniuCastig, 0, 0);
  keypad(MeniuCastig, true);

  mvwaddstr(MeniuCastig, 1, 8, "FELICITARI, AI CASTIGAT!");

  char *alegeri[2] = {"INCEPE ALT JOC", "IESI DIN PROGRAM"};
  int alegere;
  int evidentiat = 0;

  while (1)
  {
    for (int i = 0; i < 2; i++)
    {
      // Evidentiem alegerea pe care se afla programul
      if (i == evidentiat)
        wattron(MeniuCastig, A_REVERSE);
      mvwprintw(MeniuCastig, i + 3, 2, "%s", alegeri[i]);
      wattroff(MeniuCastig, A_REVERSE);
    }

    noecho();
    alegere = wgetch(MeniuCastig);
    echo();

    switch (alegere)
    {
    // Acest switch o sa ne ajute sa putem trece de la o alegere de la alta folosind sagetile de la tastatura
    case KEY_UP:
      evidentiat--;
      if (evidentiat == -1)
        evidentiat = 0;
      break;
    case KEY_DOWN:
      evidentiat++;
      if (evidentiat == 2)
        evidentiat = 1;
      break;
    default:
      break;
    }
    // Cand utilizatorul apasa enter inseamna ca a ales optiunea dorita
    if (alegere == 10)
      break;
  }

  // Observam ce a ales utilizatorul si functia va returna 1, daca utilizatorul a ales "INCEPE ALT JOC" sau va returna 0 daca a ales "IESI DIN PROGRAM"
  if (alegeri[evidentiat] == "INCEPE ALT JOC")
  {
    return 1;
  }
  else if (alegeri[evidentiat] == "IESI DIN PROGRAM")
  {
    return 0;
  }
  getch();
  wrefresh(MeniuCastig);
}

int MeniuPierdut(float mijcelulay, float START_YMP, float START_XMP, char raspuns[])
{
  WINDOW *Meniupierdut = newwin(mijcelulay * 4, LATIMEF, START_YMP, START_XMP);
  wrefresh(Meniupierdut);
  refresh();
  box(Meniupierdut, 0, 0);
  keypad(Meniupierdut, true);

  mvwaddstr(Meniupierdut, 1, 14, "AI PIERDUT!");
  mvwprintw(Meniupierdut, 2, 10, "Cuvantul era: %s", raspuns);

  char *alegeri[2] = {"INCEPE ALT JOC", "IESI DIN PROGRAM"};
  int alegere;
  int evidentiat = 0;

  while (1)
  {
    for (int i = 0; i < 2; i++)
    {
      if (i == evidentiat)
        wattron(Meniupierdut, A_REVERSE);
      mvwprintw(Meniupierdut, i + 4, 2, "%s", alegeri[i]);
      wattroff(Meniupierdut, A_REVERSE);
    }

    noecho();
    alegere = wgetch(Meniupierdut);
    echo();

    switch (alegere)
    {
    case KEY_UP:
      evidentiat--;
      if (evidentiat == -1)
        evidentiat = 0;
      break;
    case KEY_DOWN:
      evidentiat++;
      if (evidentiat == 2)
        evidentiat = 1;
      break;
    default:
      break;
    }
    if (alegere == 10)
      break;
  }

  if (alegeri[evidentiat] == "INCEPE ALT JOC")
  {
    return 1;
  }
  else if (alegeri[evidentiat] == "IESI DIN PROGRAM")
  {
    return 0;
  }
  getch();
  wrefresh(Meniupierdut);
}

int MENIU(float inaltime, float latime, float START_Y, float START_X)
{
  WINDOW *Meniu = newwin(INALTIMEF, LATIMEF, START_Y, START_X);
  wrefresh(Meniu);
  refresh();
  box(Meniu, 0, 0);
  keypad(Meniu, true);

  mvwaddstr(Meniu, 1, latime / 2 - 3, "MENIU");

  char *alegeri[2] = {"INCEPE ALT JOC", "IESI DIN PROGRAM"};
  int alegere;
  int evidentiat = 0;

  while (1)
  {
    for (int i = 0; i < 2; i++)
    {
      if (i == evidentiat)
        wattron(Meniu, A_REVERSE);
      mvwprintw(Meniu, i + 3, 2, "%s", alegeri[i]);
      wattroff(Meniu, A_REVERSE);
    }

    noecho();
    alegere = wgetch(Meniu);
    echo();
    switch (alegere)
    {
    case KEY_UP:
      evidentiat--;
      if (evidentiat == -1)
        evidentiat = 0;
      break;
    case KEY_DOWN:
      evidentiat++;
      if (evidentiat == 2)
        evidentiat = 1;
      break;
    default:
      break;
    }
    if (alegere == 10)
      break;
  }

  if (alegeri[evidentiat] == "INCEPE ALT JOC")
  {
    return 1;
  }
  else if (alegeri[evidentiat] == "IESI DIN PROGRAM")
  {
    return 0;
  }
  getch();
  wrefresh(Meniu);
}

int main()
{
restart:

  initscr();
  cbreak();

  // Declaram lista de cuvinte si alegem un cuvant random
  char *ListaCuvinte[] = {"arici", "atent", "baiat", "ceata", "debut", "peste", "fixat", "hamac",
                          "harta", "jalon", "jucam", "lacat", "magie", "nufar", "oaste", "perus",
                          "rigle", "roman", "sanie", "scris", "sonda", "texte", "tipar", "titan",
                          "zebra", "vapor", "vatra"};
  char raspuns[6];
  strcpy(raspuns, ListaCuvinte[ValoareRandom(26, 0)]);

  // Luam varoile maxime ale terminalului
  int MAXx, MAXy;
  getmaxyx(stdscr, MAXy, MAXx);

  int START_YF = 5;
  int START_XF = MAXx / 2 - 15;

  mvprintw(0, MAXx / 2, "WORDLE GAME");

  // Cream fereastra de joc
  WINDOW *win = newwin(INALTIMEF, LATIMEF, START_YF, START_XF);
  refresh();

  float mijcelulax = (LATIMEF / LITERE) / 2, mijcelulay = (INALTIMEF / INCERCARI) / 2;
  wmove(win, mijcelulay, mijcelulax);

  // Desenam tabelul
  CreareTABEL(win, START_XF, START_YF);
  wmove(win, mijcelulay, mijcelulax);
  wrefresh(win);

  float START_YMC = START_YF + INALTIMEF + 2;
  float START_XMC = START_XF;

  // Citim si verificam fiecare raspuns al utilizatorului
  int RaspunsCitire = CitireCeule(win, mijcelulay, mijcelulax, raspuns, START_XF);

  if (RaspunsCitire == 1)
  {
    // Daca am intrat aici inseamna ca utilizatorul a ghicit cuvantul si a castigat
    // Stergem fereastra jocului pentru a elibera memorie
    delwin(win);
    if (MeniuCastig(mijcelulay, START_YMC, START_XMC) == 1)
    {
      // Verificam daca utilizatorul vrea sa inceapa un joc nou
      clear();
      goto restart;
    }
  }
  else if (RaspunsCitire == 2)
  {
    // Daca am intrat aici inseamna ca utilizatorul a apasat ":" si a dorit sa deschida meniul
    if (MENIU(INALTIMEF, LATIMEF, START_YF, START_XF) == 1)
    {
      // Verificam daca utilizatorul vrea sa inceapa un joc nou
      clear();
      goto restart;
    }
  }
  else
  {
    // Daca am intrat aici inseamna ca utilizatorul a ramas fara incercari si a pierdut
    float START_YMP = START_YF + INALTIMEF + 2;
    float START_XMP = START_XF;

    delwin(win);
    if (MeniuPierdut(mijcelulay, START_YMP, START_XMP, raspuns) == 1)
    {
      // Verificam daca utilizatorul vrea sa inceapa un joc nou
      clear();
      goto restart;
    }
  }

  endwin();

  return 0;
}