#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <bits/stdc++.h>

using namespace std;
byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server_addr(192, 168, 43, 245);     // IP of the MySQL *server* here
char user[] = "Kenneths_Iphone";              // MySQL user login username
char password[] = "kenneth123sej";            // MySQL user login password
const char *SSID = "raspi-webgui";                 // Name of WiFi
const char *WiFiPassword = "ChangeMe";        //Password til WiFi
int datacheck = 0;
char query[1000];
int change = 0;
int macint = 1073421994;

// Alle vores Accespoints navn
const char internet1[] = "D-Link_AP1";
const char internet2[] = "D-Link_AP2";
const char internet3[] = "ASUS_AP3";

// Sample query

WiFiClient client;
MySQL_Connection conn((Client *)&client);
void ConnectToWiFi()
{

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WiFiPassword);
  Serial.print("Connecting to "); Serial.println(SSID);

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);

    if ((++i % 16) == 0)
    {
      Serial.println(F(" still trying to connect"));
    }
  }

  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect
  ConnectToWiFi();
  conn.connect(server_addr, 3306, user, password);
  Serial.println("Connecting...");
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");
}


void insertRSSI()
{
  String mac = WiFi.macAddress();
  int n = mac.length();
  char char_array[n + 1];
  strcpy(char_array, mac.c_str());
  for (int i = 0 ; i < n ; i++)
  {
    cout << char_array[i];
  }

  //char str[] = WiFi.macAddress();
  uint8_t bytes[6];
  int values[6];
  int i;

  if ( 6 == sscanf( char_array, "%x:%x:%x:%x:%x:%x%*c",
                    &values[0], &values[1], &values[2],
                    &values[3], &values[4], &values[5] ) )
  {
    /* convert to uint8_t */
    for ( i = 0; i < 6; ++i )
      bytes[i] = (uint8_t) values[i];
  }

  int antal = WiFi.scanNetworks();

  int * Signal;

  int RSSI_1, RSSI_2, RSSI_3;

  for (int i = 0; i < antal; ++i)
  {
    if (WiFi.SSID(i) == internet1 || WiFi.SSID(i) == internet2 || WiFi.SSID(i) == internet3 ) {
      Serial.print(WiFi.SSID(i));
      Serial.print("'s signalstyrke er ");
      Serial.println(WiFi.RSSI(i));
      if (WiFi.SSID(i) == internet1)
      {
        //int Signal1 *RSSI_1 = WiFi.RSSI(i);
        Signal = &RSSI_1;
        *Signal = WiFi.RSSI(i);
      }
      else if (WiFi.SSID(i) == internet2)
      {
        Signal = &RSSI_2;
        *Signal = WiFi.RSSI(i);
      }
      else if (WiFi.SSID(i) == internet3)
      {
        Signal = &RSSI_3;
        *Signal = WiFi.RSSI(i);
      }
    }
  }
  Serial.println();

  // Wait a bit before scanning again
  delay(1000);

  delay(2000);
  char query_string[] = {"INSERT INTO trilateration.RSSI VALUES (%i,%i,%i,%i,CURRENT_TIME  )"};
  sprintf(query, query_string, bytes, RSSI_1, RSSI_2, RSSI_3);
  Serial.println("Recording data.");

  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(query);
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
}

void updateRSSI()
{
  String mac = WiFi.macAddress();
  int n = mac.length();
  char char_array[n + 1];
  strcpy(char_array, mac.c_str());
  for (int i = 0 ; i < n ; i++)
  {
    cout << char_array[i];
  }

  uint8_t bytes[6];
  int values[6];
  int i;

  if ( 6 == sscanf( char_array, "%x:%x:%x:%x:%x:%x%*c",
                    &values[0], &values[1], &values[2],
                    &values[3], &values[4], &values[5] ) )
  {
    /* convert to uint8_t */
    for ( i = 0; i < 6; ++i )
      bytes[i] = (uint8_t) values[i];
  }

  int antal = WiFi.scanNetworks();

  int * Signal;

  int RSSI_1, RSSI_2, RSSI_3;

  for (int i = 0; i < antal; ++i)
  {
    if (WiFi.SSID(i) == internet1 || WiFi.SSID(i) == internet2 || WiFi.SSID(i) == internet3 ) {
      Serial.print(WiFi.SSID(i));
      Serial.print("'s signalstyrke er ");
      Serial.println(WiFi.RSSI(i));
      if (WiFi.SSID(i) == internet1)
      {
        //int Signal1 *RSSI_1 = WiFi.RSSI(i);
        Signal = &RSSI_1;
        *Signal = WiFi.RSSI(i);
      }
      else if (WiFi.SSID(i) == internet2)
      {
        Signal = &RSSI_2;
        *Signal = WiFi.RSSI(i);
      }
      else if (WiFi.SSID(i) == internet3)
      {
        Signal = &RSSI_3;
        *Signal = WiFi.RSSI(i);
      }
    }
  }
  Serial.println();

  // Wait a bit before scanning again
  delay(1000);



  delay(2000);
  char query_string[] = {"UPDATE trilateration.RSSI SET RSSI1 = %i, RSSI2 = %i, RSSI3 = %i, Tidspunkt = CURRENT_TIME WHERE MAC = %i"};
  sprintf(query, query_string, RSSI_1, RSSI_2, RSSI_3, macint);
  Serial.println("UPDATING.");

  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(query);
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
}

void commitRSSI()
{
  delay(2000);
  char query_string[] = {"COMMIT"};
  sprintf(query, query_string);
  Serial.println("COMMIT.");

  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(query);
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
}

void insertMAC()
{
  delay(2000);
  char query_string[] = {"INSERT INTO trilateration.Placering VALUES (%i,0,0)"};
  sprintf(query, query_string, macint);
  Serial.println("INSERTING MAC INTO Placering.");

  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(query);
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
}
void loop()
{
  while (change == 0)
  {
    insertRSSI();
    insertMAC();
    change++;
  }
  
  while (change == 1)
  {
    updateRSSI();
    commitRSSI();
  }
  
}
