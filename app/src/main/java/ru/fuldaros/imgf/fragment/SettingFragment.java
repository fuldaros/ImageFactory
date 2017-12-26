package ru.fuldaros.imgf.fragment;

import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Process;
import android.support.v7.preference.Preference;
import android.support.v7.preference.PreferenceFragmentCompat;

import ru.fuldaros.imgf.R;
import ru.fuldaros.imgf.core.ImageFactory;
import ru.fuldaros.imgf.ui.Dialog;
import ru.fuldaros.imgf.ui.Toast;
import ru.fuldaros.imgf.util.ShellUtils;
import ru.fuldaros.imgf.util.XmlDataUtils;

/**
 * Created by fuldaros on 2016/8/12.
 */
public class SettingFragment extends PreferenceFragmentCompat {

    @Override
    public void onCreatePreferences(Bundle bundle, String s) {
        addPreferencesFromResource(R.xml.setting);
    }
}
