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
////console.log('reset = ' + obj.props.reset);
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
////            console.log('|' + domPage.href + '|');
////            console.log('|' + domPage.innerText + '|');
                pages.push({'id': i, 'text': domPage.innerText, 'href': domPage.href});
            }
        }
        obj.setState({'reset': true, 'episodes': episodes, 'pages': pages});
      }
    });
}

export class FetchEpisodes extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            reset: false,
            episodes: [],
            pages: []
        };

        this.handlePageButtonClick = this.handlePageButtonClick.bind(this);
    }

    handlePageButtonClick(href) {
////    console.log('handlePageButtonClick: "' + href + '"');
        if (href !== "")
            listEpisodes(this, href);
    }

    componentDidMount() {
////    console.log('FetchEpisodes.componentDidMount: mid = "' + this.props.mid + '", title = "' + this.props.title + '"');
        listEpisodes(this, getUrl(this.props.mid, this.props.title));
    }

    componentWillReceiveProps(nextProps) {
////    console.log('FetchEpisodes.componentWillReceiveProps: mid = "' + nextProps.mid + '", title = "' + nextProps.title + '"');
        listEpisodes(this, getUrl(nextProps.mid, nextProps.title));
    }

    render() {
////    console.log('FetchEpisodes.render: mid = "' + this.props.mid + '", title = "' + this.props.title + '"');
////    console.log('FetchEpisodes.render: # of episodes = ' + this.state.episodes.length);
        return (
            <div>
              <h1>{this.props.midsMap[this.props.mid]}/{this.props.title}</h1>
              <ul>
                {this.state.episodes.map((episode) =>
                  <FetchClips key={episode.id} id={episode.id} reset={this.state.reset} title={episode.title} href={episode.href}/>
                )}
              </ul>
                {this.state.pages.map((page) =>
                  <PageButton key={page.id} id={page.id} text={page.text} href={page.href} disabled={page.href === ""} onPageButtonClick={this.handlePageButtonClick}/>
                )}
            </div>
        );
    }
}
