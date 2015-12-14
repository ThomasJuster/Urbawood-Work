package com.tj.urbawoodbeaconapp;


import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import org.altbeacon.beacon.Beacon;
import org.w3c.dom.Text;

/**
 * Created by Moi² on 26/10/2015.
 */
public final class BeaconAdapter extends ArrayAdapter<Beacon> {

    private final int beaconItemLayoutResource;

    public BeaconAdapter(final Context context, final int beaconItemLayoutResource) {
        super(context, 0);
        this.beaconItemLayoutResource = beaconItemLayoutResource;
    }

    @Override
    public View getView(final int position, final View convertView, final ViewGroup parent) {
        final View view = getWorkingView(convertView);
        final ViewHolder viewHolder = getViewHolder(view);
        final Beacon beacon = getItem(position);

        viewHolder.bluetoothName.setText(beacon.getBluetoothName());
        viewHolder.bluetoothAdress.setText(beacon.getBluetoothAddress());
        viewHolder.manufacturerId.setText(String.valueOf(beacon.getManufacturer()));
        viewHolder.RSSI.setText(String.valueOf(beacon.getRssi()));
        viewHolder.UUID.setText(beacon.getId1().toString());
        viewHolder.major.setText(beacon.getId2().toString());
        viewHolder.minor.setText(beacon.getId3().toString());

        return view;
    }

    private View getWorkingView(final View convertView) {
        View workingView = null;

        if(null == convertView) {
            final Context context = getContext();
            final LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            workingView = inflater.inflate(beaconItemLayoutResource, null);
        } else {
            workingView = convertView;
        }
        return workingView;
    }

    private ViewHolder getViewHolder(final View workingView) {
        final Object tag = workingView.getTag();
        ViewHolder viewHolder = null;

        if(null == tag || !(tag instanceof ViewHolder)) {
            viewHolder = new ViewHolder();

            viewHolder.bluetoothAdress = (TextView) workingView.findViewById(R.id.beaconBluetoothAddress);
            viewHolder.bluetoothName = (TextView) workingView.findViewById(R.id.beaconBluetoothName);
            viewHolder.manufacturerId = (TextView) workingView.findViewById(R.id.beaconManufacturer);
            viewHolder.RSSI = (TextView) workingView.findViewById(R.id.beaconRSSI);
            viewHolder.UUID = (TextView) workingView.findViewById(R.id.beaconUUID);
            viewHolder.major = (TextView) workingView.findViewById(R.id.beaconMajor);
            viewHolder.minor = (TextView) workingView.findViewById(R.id.beaconMinor);

            workingView.setTag(viewHolder);
        } else {
            viewHolder = (ViewHolder) tag;
        }

        return viewHolder;
    }

    private static class ViewHolder {
        public TextView bluetoothAdress;
        public TextView bluetoothName;
        public TextView manufacturerId;
        public TextView RSSI;
        public TextView UUID;
        public TextView major;
        public TextView minor;
    }
}
