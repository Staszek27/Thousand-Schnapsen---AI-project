#include <bits/stdc++.h>
using namespace std;

vector <char> all_suits   = {'H', 'D', 'C', 'S'};
vector <char> all_figures = {'9', 'J', 'Q', 'K', 'T', 'A'};

int val_suit(char s){
	
	int res = 100;
	for (auto &e : all_suits){
		if (e == s)
			return res;
		res -= 20;
	}
	cout << "critical error\n";
	exit(0);
}

int val_figure(char s){
	
	int value  = (s == 'J') * 2;
	value += (s == 'Q') * 3;
	value += (s == 'K') * 4;
	value += (s == 'T') * 10;
	value += (s == 'A') * 11;
	return value;	
}

int rnd(int poc, int kon){
	
	return poc + rand() % (kon - poc + 1); 
}

int round_amount(int x){
	
	return (x + 5) / 10 * 10;
}

struct Card{

	int  value;
	char figure, suit;
	
	Card(){}
	Card(char F, char S){
		// pik -> S, serce -> H, karo -> D, trefl -> C
		figure   = F;
		suit     = S;
		value    = val_figure(F);
	}
	
	int power(char contract){
		
		return (contract == suit ?
			value + val_suit(suit) :
			value
		);
	}
	
	string name(){
		
		string res;
		res += figure;
		res += suit;
		return res;
	}
	
	void print(){
		
		cout << name() << " ";
	}
};


vector <Card> all_cards(){
	
	vector <Card> res;
	for (auto F : all_figures)
		for (auto S : all_suits)
			res.push_back(Card(F, S));
	return res;
}


vector <char> all_contracts(vector <Card> &X){

	vector <char> res;
	for (auto e : all_suits){
		
		bool king  = false;
		bool queen = false;
		for (auto &card : X)
			if (card.suit == e){
				king  |= (card.figure == 'K');
				queen |= (card.figure == 'Q');
			}
		
		if (king and queen)
			res.push_back(e);
	}
	return res;
}

bool operator<(Card A, Card B){
	
	return A.value + A.suit * 10000 < B.value + B.suit * 10000;
}

char global_contract; // only for function beneath
bool oper(Card A, Card B){
	
	return A.power(global_contract) < B.power(global_contract);
}


void sort_by_power(vector <Card> &X, char contract){
	
	global_contract = contract;
	sort(X.begin(), X.end(), oper);
}

struct Player{
	
	vector <Card> hand, gone;
	vector <int> to_throw;
	int mode;
	string name;
	
	Player(vector <Card> &H, int m, char t){
		
		hand = H;
		mode = m;
		if (m == 0) name = "Random_";
		if (m == 1) name = "Player_";
		if (m == 2) name = "Bot_";
		
		
		name += t; 
	}
	
	void show_hand(string info, bool indexing = false){
		
		cout << info;
		int i = 1;
		for (auto &e : hand){
			if (indexing) 
				cout << (i ++) << ".";
			e.print();
		}
		cout << endl;
	}
	
	int auction_limit(){
		
		int res          = 0;
		int max_contract = 0;
		int cnt          = 0;
		
		for (auto e : all_suits){
			
			map <char, bool> mp;
			for (auto f : all_figures)
				mp[f] = false;
				
			for (auto f : hand)
				if (f.suit == e)
					mp[f.figure] = true;
			
			if (mp['A'] and !mp['T'] and mp['K'] and mp['Q'] and mp['J'] and mp['9']){
				res += 20 + val_suit(e); 
				cnt += 10; 
			}
			else if (mp['A'] and mp['T'] and mp['K']){
				for (auto f : hand)
					if (f.suit == e)
						res += val_figure(f.figure), cnt += 2;
				if (mp['Q']) res += val_suit(e); 
			}
			else{
				if (mp['K'] and mp['Q']) 
					max_contract = max(max_contract, val_suit(e));
				
				if (mp['A'] and mp['T'])
					res += 21, cnt += 4;
				else if (mp['A'])
					res += 11, cnt += 2;
			}
		}

		vector <int> V = {0, 2, 3, 4, 10, 11};
		for (int i = 0; i < V.size() and cnt > 0; i ++){
			int TTT = 4 - (i == 0);
			for (auto e : hand)
				if (val_figure(e.figure) == V[i])
					TTT --;
			res += min(TTT, cnt) * V[i];
			cnt -= TTT;
		}
		
		res += max_contract;
		return res / 10 * 10;
	}
	
	bool can_bid(int amount){
		
		if (mode == 0){
			return false; 
			return rnd(0, 1);
		}
		
		if (mode == 1){
			
			cout << "Czy podbijasz do " << amount << "? : 1. TAK  2. NIE  --> " << flush; 
			string x; cin >> x;
			return x == "1"; 
		}
		
		if (mode == 2){
			
			return auction_limit() >= amount;
		}
	}
	
	vector <int> give_away(){
		
		if (mode == 0){
			
			vector <int> posses;
			for (int i = 0; i < hand.size(); i ++)
				posses.push_back(i);
			random_shuffle(posses.begin(), posses.end());
			return {posses[0], posses[1]};
		}
		
		if (mode == 1){
			
			show_hand("Reka " + name + ": ", true);
			cout << "Podaj pozycje dwoch kart ktore chcesz oddac: " << flush;
			int x, y; cin >> x >> y; x --; y --;
			
			if (x == y or min(x, y) < 0 or max(x, y) >= hand.size()){
				cout << "Podane zle pozycje, sprobuj jeszcze raz." << endl;
				return give_away();
			}
			
			return {x, y};
		}
		
		if (mode == 2){
			
			return {to_throw[0], to_throw[1]};
		}
	}
	
	vector <Card> avaible_cards(vector <Card> &stak, Card &win_card){
		
		if (stak.empty())
			return hand;
		
		vector <Card> res;
		if (stak.size() == 1 or win_card.suit == stak[0].suit){
			
			for (auto e : hand)
				if (e.suit == win_card.suit
				and e.value > win_card.value)
					res.push_back(e);
			
			if (!res.empty())
				return res;
		}
		
		for (auto e : hand)
			if (e.suit == stak[0].suit)
				res.push_back(e);
		
		if (!res.empty())
			return res;
	
		return hand;
	}
	
	void delete_card(Card& to_delete){
		
		for (int i = 0; i < hand.size(); i ++)
			if (hand[i].name() == to_delete.name()){
				swap(hand[i], hand[hand.size() - 1]);
				hand.pop_back();
				return;
			}
	}
	
	Card play_card(vector <Card> &stak, Card &win_card, char contract){
		
		int it = 0;
		vector <Card> res = avaible_cards(stak, win_card);
			
		if (mode == 0){ 
			
			it = rnd(0, res.size() - 1); 
		}
		
		if (mode == 1){	
			
			show_hand("Twoja reka: ", false);
			cout << "Aktualny stos: ";
			for (auto e : stak)
				e.print();
			cout << endl;
			
			cout << "Mozliwe karty do rzucenia: ";
			for (int i = 0; i < res.size(); i ++)
				cout << i + 1 << ". ", res[i].print();
			cout << endl;
			
			cout << "Napisz pozycje karty, ktora chcesz zagrac: " << flush;
			cin >> it; it --;
				
			if (it < 0 or it >= res.size()){
				cout << "Bledne zagranie. Sprobuj jeszcze raz!\n" << flush;
				return play_card(stak, win_card, contract);
			}
		}
		
		if (mode == 2){
		
			vector <Card> out, ALL = all_cards();
			set <Card> S;
			
			for (auto e : gone)
				S.insert(e);
			for (auto e : hand)
				S.insert(e);
			for (auto e : stak)
				S.insert(e);
						
			for (auto e : ALL)
				if (!S.count(e))
					out.push_back(e);
			
			bool done = false;
			if (stak.empty()){
			
				for (int i = 0; i < res.size(); i ++){
					bool win_all = true;
					for (auto f : out)
						if (f.power(contract) > res[i].power(contract))
							win_all = false;
					if (win_all){
					
						it = i;
						done = true;
					}
				}

				if (!done){
					vector <char> K = all_contracts(res);
					if (!K.empty()){
						for (int i = 0; i < res.size() and !done; i ++)
							if (K[0] == res[i].suit and res[i].figure == 'Q'){
								it = i;
								done = true;
							}
					}
				}

				if (!done)
					for (int i = 0; i < res.size(); i ++)
						if (res[i].power(contract) < res[it].power(contract))
							it = i;
			}
			else{
						 
				for (int i = 0; i < res.size(); i ++)
					if (res[i].value > win_card.value and (!done or res[it].value > res[i].value)){
						done = true;
						it   = i;
					}		
				
				if (!done){
					for (int i = 0; i < res.size(); i ++)
						if (res[i].value < res[it].value)
							it = i;
				}
			}
		}
		
		Card ans = Card(res[it].figure, res[it].suit);
		delete_card(res[it]);
		return ans;
	}
	
	int best_amount_to_play(){
		
		int res = 0;
		for (int i = 0; i < hand.size(); i ++)
			for (int j = i + 1; j < hand.size(); j ++){
				
				vector <Card> new_hand = {};
				for (int u = 0; u < hand.size(); u ++)
					if (u != i and u != j)
						new_hand.push_back(hand[u]);
				
				Player P = (*this);
				P.hand = new_hand;
				int cand_res = P.auction_limit();
				 	
				if (cand_res >= res){
					res = cand_res;
					to_throw = {i, j};
				}
			}
		return res;
	}
	
	int new_amount(){
		
		if (mode == 0){
		
			return -1;
		}
		
		if (mode == 1){
			
			cout << "\nIle chcesz grac: " << flush;
			int x; cin >> x; x /= 10; x *= 10;
			cout << endl;
			return x; 
		}
		
		if (mode == 2){
			
			return best_amount_to_play();
		}
	}
	
};

struct Game{
	
	vector <Player> players;
	vector <Card>   stak;
	vector <int>    won_amounts, amounts;
	int games_played, p1, p2, p3;
	int who_starts;   // licytacja
	int whos_turn;    // rozgrywka
	int amount_to_play;
	char contract;
	bool printing;
	
	void init_game(){
		vector <Card> P[3];
		stak = all_cards();
		random_shuffle(stak.begin(), stak.end());
		
		for (int i = 0; i < 3; i ++)
			for (int j = 0; j < 7; j ++){
				P[i].push_back(stak.back());
				stak.pop_back();
			}
		
		players = {
			Player(P[0], p1, '1'), 
			Player(P[1], p2, '2'), 
			Player(P[2], p3, '3')
		};
	}
	
	Game(int P1, int P2, int P3, bool t){
		
		p1 = P1;
		p2 = P2;
		p3 = P3;
		won_amounts  = {0, 0, 0};
		games_played = 0;
		printing     = t;
		init_game();
	}
	
	
	bool check_hands(){
		
		for (auto &e : players){
			int sum = 0;
			for (auto &f : e.hand)
				sum += f.value;
			if (sum < 18) return false;
		}
		return true;
	}
	
	
	void auction(){
		
		amount_to_play = 100;
		who_starts = (games_played + 1) % 3;
		vector <bool> in_auction = {true, true, true};
		vector <int>  max_bid    = {120,  120,  120};
		
		if (printing){
			cout << "\n####### LICYTACJA #######\n";
			cout << "Na sto jest:      " << players[(who_starts + 2) % 3].name << endl;	
			cout << "Zaczyna licytacje " << players[who_starts].name << endl;
			for (int i = 0; i < 3; i ++)
				if (players[i].mode == 1)
					players[i].show_hand("Reka " + players[i].name + ": ");
			cout << "#########################\n\n";		
		}

		for (int i = 0; i < players.size(); i ++){
			vector <char> contracts = all_contracts(players[i].hand);
			for (auto &e : contracts)
				max_bid[i] += val_suit(e);
		}
		
		bool en = false;
		while (!en){
			for (int i = who_starts + 1; i < who_starts + 4; i ++){
				
				
				int in_game = 0;
				for (auto e : in_auction)
					in_game += e;
				
				if (in_game < 2){
					en = true;
					break;
				}
				
				int who = i % 3;
				if (!in_auction[who]) 
					continue;
				
				if (max_bid[who] <= amount_to_play){
					in_auction[who] = false; 
					continue;
				}
				
				if (!players[who].can_bid(amount_to_play + 10)){
					in_auction[who] = false;
					continue;
				}
				
				who_starts = who;
				amount_to_play += 10;
				
				if (printing)
					cout << players[who].name << " podbija do " << amount_to_play << endl;
			}
		}
		whos_turn = who_starts;
		if (printing){
			cout << "\nLicytacje wygrywa " << players[whos_turn].name;
			cout << ". Kwota do ugrania "  << amount_to_play << endl;	
		}
	}
	
	void show_stack(string info){
		
		cout << info;
		for (auto &e : stak)
			e.print();
		cout << endl;
	
	}
	
	void give_stack(){
			
		for (int i = 0; i < 3; i ++){
			players[who_starts].hand.push_back(stak.back());
			stak.pop_back();
		}	
	}
	
	void give_away_stack(){
		
		vector <int>  posses = players[who_starts].give_away();
		vector <Card> new_hand;
		
		for (int i = 0; i < players[who_starts].hand.size(); i ++)
			if (i != posses[0] and i != posses[1])
				new_hand.push_back(players[who_starts].hand[i]);
			
		for (int i = 0; i < 3; i ++)
			if (who_starts != i){
				players[i].hand.push_back(players[who_starts].hand[posses.back()]);
				posses.pop_back();
			}
		players[who_starts].hand = new_hand;
	}
	
	void show_info(){
		cout << "\n!!!!!!!!!!!!!!!!!!!!!!";
		cout << "\n      Runda " << games_played + 1 << endl;
		cout << "!!!!!!!!!!!!!!!!!!!!!!\n\n";
		for (int i = 0; i < 3; i ++)
			cout << players[i].name << ": " << won_amounts[i] << endl;
	}
	
	void check_if_win(){
		
		for (int i = 0; i < 3; i ++)
			if (won_amounts[i] == 1000){
				if (printing)
					cout << "Wygrywa gracz " << players[i].name << ", gratulacje!!!" << endl;
				else 
					cout << players[i].name << " ";
				exit(0);
			}
				
	}
	
	void set_amounts_and_contract(){
		
		amounts = {0, 0, 0};
		contract = 'X';
	}
	
	void show_amounts(){
		
		cout << "\nAktualny stan gry.\n";
		for (int i = 0; i < players.size(); i ++)
			cout << players[i].name << ": " << amounts[i] << endl;
		cout << endl;
	}
	
	void give_scores(){
		
		for (int i = 0; i < 3; i ++)
			won_amounts[i] = (
				(who_starts == i) ? (
					(amounts[i] >= amount_to_play) ?
					won_amounts[i] + amount_to_play :
					won_amounts[i] - amount_to_play
				) : 
				min(800, round_amount(won_amounts[i] + amounts[i]))
			);
		
		for (int i = 0; i < 3; i ++)
			won_amounts[i] = min(won_amounts[i], 1000);
	}
	
	
	void play_round(){
	
		int win_pos;
		Card win_card;
		
		if (printing){
			cout << "\nZaczyna " << players[whos_turn].name << endl;
			cout << "Biezacy meldunek " << contract << endl;			
		}
		
		for (int i = whos_turn; i < whos_turn + 3; i ++){
			
			int cur = i % 3;
			
			Card card_played = players[cur].play_card(stak, win_card, contract);
			
			if (i == whos_turn){
				
				win_pos  = whos_turn;
				win_card = card_played;
				
				vector <Card> with_played = players[whos_turn].hand;
				with_played.push_back(card_played);
				vector <char> X = all_contracts(with_played);
				
				for (auto e : X)
					if (e == card_played.suit 
							and (card_played.figure == 'Q'
									or card_played.figure == 'K')){
							amounts[cur] += val_suit(e);
							contract = e;
						}
			}
			else if ((win_card.suit == card_played.suit 
						and win_card.value < card_played.value)
					or (win_card.suit != contract
						and card_played.suit == contract)){
					win_pos  = cur;
					win_card = card_played;	
				}
			stak.push_back(card_played);
		}
		
		if (printing)
			show_stack("Zagrane karty w tej parti: ");
		
		whos_turn = win_pos;
		for (auto e : stak)		
			amounts[win_pos] += e.value;
			
		for (int i = 0; i < 3; i ++)
			for (auto e : stak)
				players[i].gone.push_back(e);
		stak.clear();
	}
	
	void show_hands(){
		
		for (int i = 0; i < 3; i ++){
			cout << players[i].name << ": ";
			for (auto e : players[i].hand)
				e.print();
			cout << endl;
		}
	}
	
	void play(){
		
		if (printing)
			show_info();
		auction();
		if (printing) 
			show_stack("Musik zawiera karty: ");
		give_stack();
		
		
		amount_to_play = max(
			amount_to_play,
			players[who_starts].new_amount()
		); 
		
		if (printing)
			cout << players[who_starts].name << " gra " << amount_to_play << endl;
		give_away_stack();
		set_amounts_and_contract(); 
		
		for (int i = 0; i < 8; i ++){
			//show_hands();
			play_round();
			 
			if (printing)
				show_amounts();
		}
		give_scores();
		check_if_win();
		games_played ++;
		init_game();
	}
};


void INIT(int seed){
	
	ios_base::sync_with_stdio(0);
	cout.tie(0);
	cin.tie(0);
	srand(seed);
}

int main(int argc, char *argv[]){
	
	vector <int> ARGS = {1, 0, 1, 2};
	
	for (int i = 0; i < argc - 1; i ++)
		ARGS[i] = atoi(argv[i + 1]);
	
	INIT(ARGS[0]);
	Game G(ARGS[1], ARGS[2], ARGS[3], true);
	while(1) G.play();
	
	// 0 - losowy
	// 1 - input
	// 2 - bot
}





