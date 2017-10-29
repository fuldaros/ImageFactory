package ru.fuldaros.imgf.fragment;

import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.List;

import ru.fuldaros.imgf.R;
import ru.fuldaros.imgf.activity.BaseActivity;
import ru.fuldaros.imgf.adapter.AboutAdapter;
import ru.fuldaros.imgf.bean.AboutItem;
import ru.fuldaros.imgf.ui.Dialog;
import ru.fuldaros.imgf.util.DeviceUtils;

/**
 * Created by fuldaros on 16-8-24.
 */
public class AboutFragment extends BaseFragment implements AdapterView.OnItemClickListener {

    private ListView listView;
    private AboutAdapter adapter;
    private List<AboutItem> list = new ArrayList<>();

    public static BaseFragment newInstance(BaseActivity activity) {
        AboutFragment fragment = new AboutFragment();
        fragment.setActivity(activity);
        return fragment;
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View root = getContentView();
        if (root == null) {
            root = inflater.inflate(R.layout.fragment_about, container, false);
            setContentView(root);
            listView = (ListView) findViewById(R.id.about_listview);
            list.add(new AboutItem("Android Open Source Project", "Google", "Android is an open source software stack for a wide range of mobile devices and a corresponding open source project led by Google. This site offers the information and source code you need to create custom variants of the Android stack, port devices and accessories to the Android platform, and ensure your devices meet compatibility requirements.", "https://source.android.com/"));
            adapter = new AboutAdapter(getActivity(), list);
            listView.setOnItemClickListener(this);
            listView.setAdapter(adapter);
            findViewById(R.id.me).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    DeviceUtils.openUrl(getActivity(), "http://4pda.ru");
                }
            });
        }
        return root;
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        final String url = list.get(position).getUrl();
        Dialog.create(getActivity()).setTitle(R.string.browse_home_page)
                .setMessage(url)
                .setPositiveButton(R.string.browse, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        DeviceUtils.openUrl(getActivity(), url);
                    }
                })
                .setNegativeButton(android.R.string.cancel, null)
                .show();
    }

}
