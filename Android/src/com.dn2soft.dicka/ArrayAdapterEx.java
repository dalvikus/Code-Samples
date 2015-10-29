package com.dn2soft.dicka;

import android.widget.ArrayAdapter;
import com.dn2soft.dicka.LoadActivity.Entry;
import java.util.List;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.ViewGroup;
import android.view.View;
import android.widget.TextView;

/**
 * to override getView
 */
public class ArrayAdapterEx extends ArrayAdapter<Entry> {
    private final LayoutInflater mInflater;

    public static int TYPE_ONE_ITEM = 1;
    public static int TYPE_TWO_ITEMS = 2;

    final int mType;
    public ArrayAdapterEx(Context context, int iType) {
        super(context, iType == ArrayAdapterEx.TYPE_ONE_ITEM ? android.R.layout.simple_list_item_1 : android.R.layout.simple_list_item_2);
        mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mType = iType;
    }

    public void setData(List<Entry> entries) {
        clear();
        if (entries != null) {
            addAll(entries);
        }
    }
    @Override
    public View getView(int position, View convertView, ViewGroup parent)
    {
        View view;

        if (convertView == null) {
            view = mInflater.inflate(mType == ArrayAdapterEx.TYPE_ONE_ITEM ? android.R.layout.simple_list_item_1 : android.R.layout.simple_list_item_2, parent, false);
        } else {
            view = convertView;
        }

        Entry item = getItem(position);
        if (mType == ArrayAdapterEx.TYPE_ONE_ITEM) {
            ((TextView) view.findViewById(android.R.id.text1)).setText(item.getWord());
        } else {
            ((TextView) view.findViewById(android.R.id.text1)).setText(item.getWord());
            ((TextView) view.findViewById(android.R.id.text2)).setText(item.getSense());
        }

        return view;
    }
}
