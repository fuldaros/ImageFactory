package ru.fuldaros.imgf.activity;

import android.support.v7.widget.Toolbar;
import android.view.MenuItem;

import ru.fuldaros.imgf.R;

/**
 * Created by fuldaros on 2017/10/28.
 */
public class BaseChildActivity extends BaseActivity {

    public void applyToolbar(int id) {
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setHomeButtonEnabled(true);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (getSupportActionBar() != null) {
            if (item.getItemId() == android.R.id.home) {
                this.finish();
            }
        }
        return super.onOptionsItemSelected(item);
    }
}
