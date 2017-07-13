import React from 'react';
import axios from 'axios';

import {FetchClips} from "./FetchClips";
import {PageButton} from "./PageButton";
import {urlViaCorsProxy} from "./Constants";

function getUrl(mid, title)
{
    return 'https://baykoreans.link/?act=&vid=&mid=' + mid + '&category=&search_target=title&search_keyword=' + encodeURIComponent(title);
}

function listEpisodes(obj, url)
{
////console.log('url: |' + url + '|');
////console.log('resetClips = ' + obj.resetClips);
    const hostRe = '//(.+?)/';
    let a = url.match(hostRe);
    if (a === null) {
        console.error('Invalid url: "' + url + '"');
        return;
    }
    let host = a[1];
    const request = axios.get(urlViaCorsProxy(url));
    request.then(response => {
      if (response.status !== 200) {
        console.error(response.status);
        console.error(response.statusText);
        console.error(response.headers);
      } else {
        let doc = new DOMParser().parseFromString(response.data, "text/html");

        let domEpisodes = doc.querySelectorAll("td.title a");
        let episodes = [];
        for (let i = 0; i < domEpisodes.length; ++i) {
            let domEpisode = domEpisodes[i];
////        console.log('[' + i + ']: |' + domEpisode.href + '|');
////        console.log('[' + i + ']: |' + domEpisode.innerText + '|');
            if (domEpisode.href.endsWith("#comment"))
                continue;
            episodes.push({'id': i, 'title': domEpisode.innerText, 'href': domEpisode.href});
        }
////    console.log('# of episodes = ' + domEpisodes.length);

        let pages = [];
        if (domEpisodes.length > 0) {
            let domPages = doc.querySelectorAll("ul.pagination li a");
            for (let i = 0; i < domPages.length; ++i) {
                let domPage = domPages[i];
                let href = domPage.href;
                if (href !== '') {
                    let a1 = href.match(hostRe);
                    if (a1 === null || host !== a1[1])
                        href = '';
                }
////            console.log('|' + domPage.href + '|; href = "' + href + '"');
////            console.log('|' + domPage.innerText + '|');
                pages.push({'id': i, 'text': domPage.innerText, 'href': href});
            }
        }
        obj.resetClips = true;
        obj.setState({'episodes': episodes, 'pages': pages});
      }
    });
}

export class FetchEpisodes extends React.Component {
    constructor(props) {
        super(props);

        /// resetClips is set only when listEpisodes is called, where setState is called,
        /// which means that render is called only if resetClips is true (at shouldComponentUpdate)
        /// so after render, it should be reset to false (at componentDidUpdate)
        this.resetClips = false;
        this.state = {
            episodes: [],
            pages: []
        };

        this.handlePageButtonClick = this.handlePageButtonClick.bind(this);
    }

    handlePageButtonClick(href) {
        if (href !== "")
            listEpisodes(this, href);
    }

    componentDidMount() {
        listEpisodes(this, getUrl(this.props.mid, this.props.title));
    }

    componentWillReceiveProps(nextProps) {
        if (nextProps.mid !== this.props.mid || nextProps.title !== this.props.title)
            listEpisodes(this, getUrl(nextProps.mid, nextProps.title));
    }

    shouldComponentUpdate(nextProps, nextState) {
        return this.resetClips;
    }
    componentDidUpdate(prevProps, prevState) {
        this.resetClips = false;
    }

    render() {
        return (
            <div id="content-div">
              <h1 id="mid-title-h1"><div id="mid-div">{this.props.midsMap[this.props.mid]}</div><div id="mid-title-separator">/</div><div id="title-div">{this.props.title}</div></h1>
              <ul id="episodes-ul">
                {this.state.episodes.map((episode) =>
                  <FetchClips key={episode.id} id={episode.id} reset={this.resetClips} title={episode.title} href={episode.href}/>
                )}
              </ul>
              <div id="pages-div">
                {this.state.pages.map((page) =>
                  <PageButton key={page.id} id={page.id} text={page.text} href={page.href} disabled={page.href === ""} onPageButtonClick={this.handlePageButtonClick}/>
                )}
              </div>
            </div>
        );
    }
}
