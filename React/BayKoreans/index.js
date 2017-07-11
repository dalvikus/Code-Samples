import React from 'react';
import ReactDOM from 'react-dom';

import {midsMap} from './Constants';
import {loadBookmarks} from "./Bookmarks";
import {BayKoreans} from "./BayKoreans";

ReactDOM.render(
  <BayKoreans midsMap={midsMap} bookmarks={loadBookmarks()}/>,
  document.getElementById('root')
);
