package com.tj.urbawoodbeaconapp;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.os.Bundle;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.ListView;
import android.widget.TextView;

import org.altbeacon.beacon.Beacon;
import org.altbeacon.beacon.BeaconConsumer;
import org.altbeacon.beacon.BeaconManager;
import org.altbeacon.beacon.BeaconParser;
import org.altbeacon.beacon.RangeNotifier;
import org.altbeacon.beacon.Region;
import org.altbeacon.beacon.service.BeaconService;
import org.w3c.dom.Text;

import java.io.BufferedInputStream;
import java.io.InputStream;
import java.lang.reflect.Array;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;

public class HomeActivity extends Activity implements BeaconConsumer {

    ListView beaconsList = null;
    BeaconAdapter beaconAdapter = null;
    TextView noBeacon = null;
    TextView roomUserIsIn = null;

    int nbValeurs = 0;
    String names[] = new String[5];
    String closestBeacon;

    //String urlNodeServer = new String("http://10.134.15.31:3100/closestbeacon/");
    String urlNodeServer = "http://cs.isen.fr/lab/beacon_manager/";
    String query = "";

    BeaconManager beaconManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);

        requestToTurnOnBluetooth();

        instantiateViews();

        //Beacon
        initiateBeaconManager();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        beaconManager.unbind(this);
    }

    private void requestToTurnOnBluetooth() {
        // Request to put on the Bluetooth :
        int REQUEST_ENABLE_BT=1;
        BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if(mBluetoothAdapter == null) {
            return;
        }
        if(!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
    }

    private void instantiateViews () {
        beaconsList = (ListView) findViewById(R.id.beaconsList);
        beaconAdapter = new BeaconAdapter(this, R.layout.beacon_list_item);
        beaconsList.setAdapter(beaconAdapter);
        noBeacon = (TextView) findViewById(R.id.noBeacon);
        roomUserIsIn = (TextView) findViewById(R.id.roomUserIsIn);
    }

    private void initiateBeaconManager () {
        beaconManager = BeaconManager.getInstanceForApplication(this);
        beaconManager.getBeaconParsers().add(new BeaconParser().setBeaconLayout("m:2-3=0215,i:4-19,i:20-21,i:22-23,p:24-24"));
        beaconManager.bind(this);
    }

    //Beacon implementation
    public void onBeaconServiceConnect() {

        beaconManager.setRangeNotifier(new RangeNotifier() {
            @Override
            public void didRangeBeaconsInRegion(Collection<Beacon> beacons, Region region) {
                if (beacons.size() > 0) {
                    // Des beacons sont détectés
                    setNoBeaconVisible(false);

                    // Trouver le beacon le plus proche
                    findClosestBeacon(beacons);

                    //Mise à jour de la ListView
                    addBeaconItems(beacons);

                    sendBathroomBeaconInfo(beacons);
                } else {
                    // Pas de beacon visible
                    setNoBeaconVisible(true);
                }
            }
        });
        try {
            beaconManager.startRangingBeaconsInRegion(new Region("myRangingUniqueId", null, null, null));
        } catch (RemoteException e) {
            Log.e("Exception", e.toString());
        }
    }


    private void callNodeServer () {
        final Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                URL url;
                HttpURLConnection urlConnection = null;
                try {
                    //url = new URL(urlNodeServer + closestBeacon);
                    url = new URL(urlNodeServer + query);
                    Log.i("Url", urlNodeServer + query);
                    urlConnection = (HttpURLConnection) url.openConnection();
                    InputStream in = new BufferedInputStream(urlConnection.getInputStream());
                }
                catch (Exception e) {
                    Log.e("Exception : ", e.toString());
                }
                finally {
                    urlConnection.disconnect();
                }
            }
        });
        t.start();
    }

    private void findClosestBeacon(Collection<Beacon> beacons) {
        if (nbValeurs < 5) {
            nbValeurs++;
            int rssi = -200;
            for (final Beacon beacon : beacons) {
                if (beacon.getRssi() > rssi) {
                    rssi = beacon.getRssi();
                    names[nbValeurs-1] = beacon.getBluetoothName();
                }
            }
        } else {
            String previousClosestBeacon = closestBeacon;
            nbValeurs = 0;
            // Results compte combien de fois chaque nom revient
            int results[] = new int[5];
            int i = 0;
            for (String name : names) {
                for (String name2 : names) {
                    if (name.equals(name2)) {
                        results[i]++;
                    }
                }
                results[i]--; // On évite qu'il ne se compte lui-même en plus.
                i++;
            }
            String result = names[0];
            for (i = 0; i < results.length - 1; i++) {
                if (results[i + 1] > results[i]) {
                    result = names[i+1];
                }
            }
            Log.i("Closest Beacon", "Résultat = " + result);
            closestBeacon = new String(result);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    //roomUserIsIn.setText("Beacon le plus proche : " + closestBeacon);
                    roomUserIsIn.setText("Emplacement : " + findAssociatedRoom(closestBeacon));
                }
            });
            if(previousClosestBeacon == null || !previousClosestBeacon.equals(closestBeacon)) {
                // Informer le serveur de la pièce dans laquelle l'utilisateur se trouve
                if(closestBeacon.equals("BEACON 39:C3:8C")) {
                    closestBeacon = closestBeacon.replaceAll(" ","+");
                    closestBeacon = closestBeacon.replaceAll(":","-");
                    previousClosestBeacon = closestBeacon;
                }
                //callNodeServer();
            }
        }
    }
    private void addBeaconItems(Collection<Beacon> beaconsToAdd) {
        if(beaconAdapter != null && beaconAdapter.getCount()>0) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    beaconAdapter.clear();
                }
            });
        }

        Collection<Beacon> beacons = new ArrayList<>(beaconsToAdd);
        String sortArray[] = new String[beacons.size()];
        int i = 0;
        for(final Beacon beacon : beacons) {
            sortArray[i] = beacon.getBluetoothName();
            i++;
        }
        Arrays.sort(sortArray);
        for(i = 0; i < sortArray.length ; i++) {
            for (final Beacon beacon : beacons) {
                if(beacon.getBluetoothName().equals(sortArray[i])) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            beaconAdapter.add(beacon);
                        }
                    });
                    beacons.remove(beacon);
                    break;
                }
            }
        }
    }
    private void setNoBeaconVisible(boolean visible) {
        if(visible) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    noBeacon.setVisibility(View.VISIBLE);
                }
            });
        } else {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    noBeacon.setVisibility(View.INVISIBLE);
                }
            });
        }
    }

    private String findAssociatedRoom(String beaconName) {
        switch(beaconName) {
            case "B-IO7Jta":
            return "Salle de réunion";

            case "B-f5G3MA":
            return "Cuisine";

            case "BEACON 39:C3:8C":
            return "Salle de bains";

            case "B-NFLzAx":
            return "Chambre";

            case "B-JXAuC8":
            return "Atelier";

            case "B-ba6DEk":
            return "Laboratoire";

            case "B-9PBBYb":
            return "Salon";

            default:
            return "Inconnu";
        }
    }

    private void sendBathroomBeaconInfo(Collection<Beacon> beacons) {
        Log.i("T","OK sendBath");
        for(final Beacon beacon : beacons) {
            if(beacon.getBluetoothName().equals("BEACON 39:C3:8C")) {
                defineQuery(beacon);
                callNodeServer();
                break;
            }
        }
    }
    private void defineQuery(Beacon beacon) {
        query = "?";
        query+= "d="+String.valueOf(beacon.getDistance()*7/3).substring(0,5) + "&";
        query+= "Bname=" + beacon.getBluetoothName() + "&";
        query+= "Baddress=" + beacon.getBluetoothAddress() + "&";
        query+= "manufacturer=" + String.valueOf(beacon.getManufacturer()) + "&";
        query+= "rssi=" + String.valueOf(beacon.getRssi()) + "&";
        query+= "TxPower=" + String.valueOf(beacon.getTxPower()) + "&";
        query+= "uuid=" + String.valueOf(beacon.getId1()) + "&";
        query+= "major=" + String.valueOf(beacon.getId2()) + "&";
        query+= "minor=" + String.valueOf(beacon.getId3());

        query = query.replace(" ", "%20");

        Log.i("Query", urlNodeServer + query);
    }
}
