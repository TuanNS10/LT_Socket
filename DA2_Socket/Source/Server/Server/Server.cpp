// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"
#include "afxsock.h"
#include "math.h"
#include <vector>
#include <string>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			CSocket server;
			unsigned int port = 12345;
			int i;
			AfxSocketInit(NULL);

			server.Create(port);
			server.Listen(5);

			//Nhap so luong client
			cout << "\n Nhap so luong Client : ";
			int num_client;
			cin >> num_client;

			cout << "\n Dang lang nghe ket noi tu Client...\n";

			//Tao mang chua cac socket client
			CSocket * sockClients = new CSocket[num_client];
			for (i = 0; i < num_client; i++){
				server.Accept(sockClients[i]);

				cout << "Da tiep nhan client " << i + 1 << "/" << num_client << endl;

				//Bao cho client biet minh la client thu may
				sockClients[i].Send((char*)&i, sizeof(int), 0);
			}

			string * NameClient = new string[num_client];
			char user[100] = "Nhap Email User: ";
			int lenguser;

			for (int i = 0; i < num_client; i++)
			{

				//gui thong tin cho nguoi dung

				lenguser = strlen(user);

				//gui di dodai thong diep
				sockClients[i].Send(&lenguser, sizeof(lenguser), 0);

				//gui thong diep
				sockClients[i].Send(user, lenguser, 0);

				int MsgSize;
				char *temp;
				//nhan client
				sockClients[i].Receive((char*)&MsgSize, sizeof(int), 0);
				temp = new char[MsgSize + 1];
				sockClients[i].Receive((char*)temp, MsgSize, 0);
				//in ra thong deip tu client
				temp[MsgSize] = '\0';
				NameClient[i] = temp;
				cout << "Email: " << NameClient[i] << endl;
				delete temp;
			}

			//kiem tra trung ten
			int check = -1;
			for (int i = num_client - 1; i >= 0; i--)
			{
				if (i == 0)
				{
					check = -1;
					sockClients[i].Send((char *)&check, sizeof(int), 0);
					break;
				}
				for (int j = i - 1; j >= 0; j--)
				{
					if (NameClient[i].compare(NameClient[j]) == 0)
					{
						check = i;
						break;
					}
				}
				if (check != -1)
				{
					sockClients[i].Send((char *)&check, sizeof(int), 0);
					//nhan client lai trung ten
					int MsgSize;
					char *temp;
					sockClients[i].Receive((char*)&MsgSize, sizeof(int), 0);
					temp = new char[MsgSize + 1];
					sockClients[i].Receive((char*)temp, MsgSize, 0);
					//in ra thong deip tu client
					temp[MsgSize] = '\0';
					NameClient[i] = temp;
					cout << "Ten moi cua client thu " << check + 1 << ": " << NameClient[i] << endl;
				}
				else if (check == -1)
				{
					sockClients[i].Send((char *)&check, sizeof(int), 0);
				}
			}

			for (int i = 0; i < num_client; i++)
			{
				cout << "Email: " << NameClient[i] << endl;
			}

			struct Hang
			{
				int id;
				char* name;
				unsigned int tien;
			}; typedef Hang hang;

			hang *arr = new hang[15]; //so luong hang
			ifstream file;
			file.open("SanPhamDauGia.txt");

			int count = 0;
			string s;

			int temp = 0;
			hang h;
			while (!file.eof())
			{

				getline(file, s);
				char* a = new char[s.size() + 1];
				strcpy(a, s.c_str());

				arr[temp].id = atoi(a);

				getline(file, s);
				a = new char[s.size() + 1];
				strcpy(a, s.c_str());

				arr[temp].name = a;

				getline(file, s);
				a = new char[s.size() + 1];
				strcpy(a, s.c_str());

				arr[temp].tien = atoi(a);
				//cout << s << endl;
				//fflush(stdout);
				cout << endl << arr[temp].id;
				cout << endl << arr[temp].name;
				cout << endl << arr[temp].tien;
				temp++;
				count++;

			}

			for (int i = 0; i < num_client; i++)
			{
				sockClients[i].Send((char*)&count, sizeof(int), 0);
				for (int j = 0; j < count; j++)
				{
					int dodaisanpham = strlen(arr[j].name);
					///
					sockClients[i].Send((char*)&arr[j].id, sizeof(int), 0);
					//gui di dodai thong diep
					sockClients[i].Send((char*)&dodaisanpham, sizeof(dodaisanpham), 0);
					//gui thong diep
					sockClients[i].Send((char*)arr[j].name, dodaisanpham, 0);

					sockClients[i].Send((char*)&arr[j].tien, sizeof(int), 0);

				}
			}

			// Struct mua hang 
			struct Muahang
			{
				int id;
				string tien;
				int id_client;
			}; typedef Muahang muahang;

			// Dau gia san pham 
			// Nhan thong tin tu cac client dua ra
			int len_method_pro;
			char * method_pro;
			string * Code_Buy = new string[100];
			muahang * arr_muahang = new muahang[100];

			time_t baygio = time(0);
			tm *ltm = localtime(&baygio);

			int phuthientai = ltm->tm_min;
			int giayhientai = ltm->tm_sec;

			char guithongbaodaugia[] = "Moi ban nhap cu phap dau gia san pham(<ID>_<Gia tien>)";
			int doidaidaugia;

			// tao bien luu
			int count_id = 0;
			int p = 1;


			int check_break_loop = 0;
			int check_log_time = 0;
			do
			{

				time_t baygio = time(0);
				tm *ltm = localtime(&baygio);
				if (phuthientai + 1 == ltm->tm_min && ltm->tm_sec > giayhientai || phuthientai + 2 <= ltm->tm_min)
				{
					check_break_loop = 1;
				}

				p %= num_client;
				//gui bien kiem tra thoi gian dau gia
				sockClients[p].Send((char*)&check_break_loop, sizeof(int), 0);
				if (check_break_loop == 0)
				{
					//gui thong tin cho nguoi dung

					doidaidaugia = strlen(guithongbaodaugia);

					//gui di dodai thong diep
					sockClients[p].Send(&doidaidaugia, sizeof(doidaidaugia), 0);

					//gui thong diep
					sockClients[p].Send(guithongbaodaugia, doidaidaugia, 0);

					int MsgSize1;
					char *temp1;

					//nhan client
					sockClients[p].Receive((char*)&MsgSize1, sizeof(int), 0);
					temp1 = new char[MsgSize1 + 1];
					sockClients[p].Receive((char*)temp1, MsgSize1, 0);
					//in ra thong deip tu client
					temp1[MsgSize1] = '\0';
					Code_Buy[count_id] = temp1;

					// kiem tra mat hang co ton tai hay khong
					arr_muahang[count_id].id = Code_Buy[count_id][0] - 48;
					int check_non_duplicate = 0;
					for (int i = 0; i < count; i++)
					{
						if (arr_muahang[count_id].id == arr[i].id)
						{
							check_non_duplicate = 0;
							break;
						}
						else
						{
							check_non_duplicate++;
						}
					}

					if (check_non_duplicate == count)
					{
						sockClients[p].Send(&check_non_duplicate, sizeof(int), 0);

						//nhan client
						sockClients[p].Receive((char*)&MsgSize1, sizeof(int), 0);
						temp1 = new char[MsgSize1 + 1];
						sockClients[p].Receive((char*)temp1, MsgSize1, 0);
						//in ra thong deip tu client
						temp1[MsgSize1] = '\0';
						Code_Buy[count_id] = temp1;

						check_non_duplicate = 0;
					}
					else
					{
						sockClients[p].Send(&check_non_duplicate, sizeof(int), 0);

						for (int j = 2; j < Code_Buy[p].length(); j++)
						{
							arr_muahang[count_id].tien += Code_Buy[count_id][j];
						}

						arr_muahang[count_id].id_client = p;
					}

					p++;
					count_id++;
					delete temp1;
				}

			} while (check_break_loop == 0);
			cout << endl;
			cout << "\nHet Gio Dau Gia: " << endl;
			sockClients[(p + 1) % num_client].Send((char *)&check_break_loop, sizeof(int), 0);


			//cout << "Danh Sach dau gia: " << endl;
			//for (int i = 0; i < count_id; i++)
			//{
			//cout << Code_Buy[i] << endl;
			//}


			/*cout << "Danh Sach Ma Hang Mua: ";
			for (int i = 0; i < count_id; i++)
			{
			cout << "==================" << endl;
			cout << arr_muahang[i].id << endl;
			cout << arr_muahang[i].tien << endl;
			cout << arr_muahang[i].id_client << endl;
			}*/

			//Thuc hien kiem tra dau gia 
			int descrease_id = 0;
			int count_arr_muahang = 0;

			int *idclient = new int[10];
			int *sanpham = new int[10];

			for (int i = 0; i < count_id; i++)
			{
				for (int j = i + 1; j < count_id; j++)
				{
					if (arr_muahang[i].id == arr_muahang[j].id)
					{
						if (arr_muahang[i].tien < arr_muahang[j].tien)
						{
							arr_muahang[i].id_client = arr_muahang[j].id_client;

						}
						arr_muahang[j].id = descrease_id;
						descrease_id--;
					}
				}
				if (arr_muahang[i].id >= 0)
				{
					cout << "San Pham " << arr_muahang[i].id << " Win: " << arr_muahang[i].id_client << endl;
					idclient[count_arr_muahang] = arr_muahang[i].id_client;
					sanpham[count_arr_muahang] = arr_muahang[i].id;
					count_arr_muahang++;
				}
			}
			//////người chiến thắng
			//cout << "Danh Sach Nguoi Chien Thang: " << endl;
			
			/*/// gửi cho từng client kết quả thắng thua
			for (int i = 0; i < count_arr_muahang; i++)
			{
				cout << idclient[i] << "/" << sanpham[i] << endl;
			}*/
		

			// tao bien kiem tra nguoi chien thang 
			for (int i = 0; i < num_client; i++)
			{
				sockClients[i].Send((char *)&num_client, sizeof(int), 0);
			}

			for (int i = 0; i < num_client; i++)
			{
				sockClients[i].Send((char*)&count_arr_muahang, sizeof(int), 0);
				for (int j = 0; j < count_arr_muahang; j++)
				{
					sockClients[i].Send((char *)&idclient[j], sizeof(int), 0);
				}
			}

			// nhan thong tin ma the ngan hang 
			char *tempcode_bank;
			int length_of_code_bank;

			char *tempcar;
			int length_of_seri_card;

			struct DS_Ma_Win
			{
				string Ma;
				string Seri;
				int id;

			};
			DS_Ma_Win *Win = new DS_Ma_Win[10];
			DS_Ma_Win *Lose = new DS_Ma_Win[10];
			int slwin = 0;
			int sllose = 0;
			

			int *giu_lai_vi_tri_thang = new int[10];

			for (int i = 0; i < num_client; i++)
			{
				// Win
				if (i == idclient[0] || i == idclient[1] || i == idclient[2] || i == idclient[3] || i == idclient[4])
				{

					//nhan client
					sockClients[i].Receive((char*)&length_of_code_bank, sizeof(int), 0);
					tempcode_bank = new char[length_of_code_bank + 1];
					sockClients[i].Receive((char*)tempcode_bank, length_of_code_bank, 0);
					//in ra thong deip tu client
					tempcode_bank[length_of_code_bank] = '\0';
					//cout << "Ma: " << tempcode_bank;
					Win[slwin].Ma = tempcode_bank;

					//nhan client
					sockClients[i].Receive((char*)&length_of_seri_card, sizeof(int), 0);
					tempcar = new char[length_of_seri_card + 1];
					sockClients[i].Receive((char*)tempcar, length_of_seri_card, 0);
					//in ra thong deip tu client
					tempcar[length_of_seri_card] = '\0';
					//cout << " Seri: " << tempcar;
					Win[slwin].Seri = tempcar;


					Win[slwin].id = i;
					slwin++;

				}
				else
				{
					//nhan client
					sockClients[i].Receive((char*)&length_of_code_bank, sizeof(int), 0);
					tempcode_bank = new char[length_of_code_bank + 1];
					sockClients[i].Receive((char*)tempcode_bank, length_of_code_bank, 0);
					//in ra thong deip tu client
					tempcode_bank[length_of_code_bank] = '\0';
					//cout << "Ma: " << tempcode_bank;
					Lose[sllose].Ma = tempcode_bank;

					//nhan client
					sockClients[i].Receive((char*)&length_of_seri_card, sizeof(int), 0);
					tempcar = new char[length_of_seri_card + 1];
					sockClients[i].Receive((char*)tempcar, length_of_seri_card, 0);
					//in ra thong deip tu client
					tempcar[length_of_seri_card] = '\0';
					//cout << " Seri: " << tempcar;
					Lose[sllose].Seri = tempcar;
					Lose[sllose].id = i;
					sllose++;
				}
			}

			// Danh Sach Khach Hang Win
			cout << "Danh Sach Mua Hang: " << endl;
			for (int i = 0; i < slwin; i++)
			{
				cout << "Client: " << Win[i].id + 1 << "\nMa Ngan Hang: " << Win[i].Ma << "\nSeri: " << Win[i].Seri << endl;
			}

			int n = count - count_arr_muahang;
			int end_line = 1;
			freopen("SanPhamChuaDauGia.txt", "wt", stdout);
			for (int i = 0; i < count; i++)
			{
				int check_duplicate_pro = 0;
				for (int j = 0; j < count_arr_muahang; j++)
				{
					if (arr[i].id == sanpham[j])
					{
						check_duplicate_pro = 0;
						break;
					}
					else
					{
						check_duplicate_pro++;
					}
				}
				if (check_duplicate_pro == count_arr_muahang)
				{
					cout << arr[i].id << " ";
					cout << arr[i].name << " ";
					cout << arr[i].tien;
					if (end_line < n)
					{
						cout << endl;
					}
					end_line++;

				}
			}
			sockClients[i].Close();
			server.Close();
		}
		
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
