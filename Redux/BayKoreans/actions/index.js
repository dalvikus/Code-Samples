export const ADD_BOOKMARK = 'ADD_BOOKMARK'
export const DELETE_BOOKMARK = 'DELETE_BOOKMARK'

export const FETCH_SHOW = 'FETCH_SHOW'
export const INVALIDATE_SHOW = 'INVALIDATE_SHOW'
export const REQUEST_EPISODES = 'REQUEST_EPISODES'
export const RECEIVE_EPISODES = 'RECEIVE_EPISODES'

export const TOGGLE_HIDE_CLIPS = 'TOGGLE_HIDE_CLIPS'
export const REQUEST_CLIPS = 'REQUEST_CLIPS'
export const RECEIVE_CLIPS = 'RECEIVE_CLIPS'

export const addBookmark = (show) => ({
  type: ADD_BOOKMARK,
  show
})

export const deleteBookmark = (index) => ({
    type: DELETE_BOOKMARK,
    index
})

export const fetchShow = (show) => ({
    type: FETCH_SHOW,
    show
})

export const invalidateShow = show => ({
  type: INVALIDATE_SHOW,
  show
})

export const requestEpisodes = show => ({
  type: REQUEST_EPISODES,
  show
})

export const receiveEpisodes = (show, episodes, pages) => ({
  type: RECEIVE_EPISODES,
  show,
  episodes,
  pages
})


export const toggleHideClips = (show, href) => ({
  type: TOGGLE_HIDE_CLIPS,
  show,
  href
})

export const requestClips = (show, href) => ({
  type: REQUEST_CLIPS,
  show,
  href
})

export const receiveClips = (show, href, clips) => ({
  type: RECEIVE_CLIPS,
  show,
  href,
  clips
})

function getUrl(show)
{
    return 'https://baykoreans.link/?act=&vid=&mid=' + show.mid + '&category=&search_target=title&search_keyword=' + encodeURIComponent(show.title);
}

export function urlViaCorsProxy(url) {
    // Cross-origin resource sharing
    //  https://en.wikipedia.org/wiki/Cross-origin_resource_sharing
    // google "cors proxy"
    const corsProxy = 'http://cors-anywhere.herokuapp.com/'
    return corsProxy + url
}
export const fetchEpisodes = (show, url) => dispatch => {
    dispatch(requestEpisodes(show))
    const hostRe = '//(.+?)/';
    let a = url.match(hostRe);
    if (a === null) {
        console.error('Invalid url: "' + url + '"');
        return;
    }
    let host = a[1];
    fetch(urlViaCorsProxy(url))
    .then(function(response) {
        if(response.status === 200) {
            return response.text();
        } else {
            throw new Error('Something went wrong on api server!');
        }
    })
    .then(function(data) {
        const doc = new DOMParser().parseFromString(data, "text/html");

        const domEpisodes = doc.querySelectorAll("td.title a");
        let episodes = [];
        for (let i = 0; i < domEpisodes.length; ++i) {
            const domEpisode = domEpisodes[i];
////        console.log('[' + i + ']: |' + domEpisode.href + '|');
////        console.log('[' + i + ']: |' + domEpisode.innerText + '|');
            if (domEpisode.href.endsWith("#comment"))
                continue;
            episodes.push({'href': domEpisode.href, 'text': domEpisode.innerText});
        }
////    console.log('# of episodes = ' + domEpisodes.length);

        let pages = [];
        if (domEpisodes.length > 0) {
            const domPages = doc.querySelectorAll("ul.pagination li a");
            for (let i = 0; i < domPages.length; ++i) {
                const domPage = domPages[i];
                let href = domPage.href;
                if (href !== '') {
                    const a1 = href.match(hostRe);
                    if (a1 === null || host !== a1[1])
                        href = '';
                }
////            console.log('|' + domPage.href + '|; href = "' + href + '"');
////            console.log('|' + domPage.innerText + '|');
                pages.push({'href': href, 'text': domPage.innerText});
            }
        }
////    console.log('# of pages = ' + pages.length);
        dispatch(receiveEpisodes(show, episodes, pages))
    })
/*
  return fetch(`https://www.reddit.com/r/${reddit}.json`)
    .then(response => response.json())
    .then(json => dispatch(receivePosts(reddit, json)))
 */
}

const shouldFetchEpisodes = (state, show) => {
  const showInfo = state.episodesByShow[show.toString()]
  if (!showInfo) {
    return true
  }
  if (showInfo.isFetching) {
    return false
  }
  return showInfo.invalidated
}

export const fetchEpisodesIfNeeded = show => (dispatch, getState) => {
  if (shouldFetchEpisodes(getState(), show)) {
    return dispatch(fetchEpisodes(show, getUrl(show)))
  }
}

export const fetchClips = (show, href, url) => dispatch => {
    dispatch(requestClips(show, href))
////console.log('fetchClips: ', show, 'href: "' + href + '"');
    fetch(urlViaCorsProxy(href))
    .then(function(response) {
        if(response.status === 200) {
            return response.text();
        } else {
            throw new Error('Something went wrong on api server!');
        }
    })
    .then(function(data) {
        const doc = new DOMParser().parseFromString(data, "text/html");
        // class="button red xLarge"
        // class="button blue xLarge"
        const links = doc.querySelectorAll("a[class^=button][class$=xLarge]");
////    console.log(links);
        const clips = [];
        for (let i = 0; i < links.length; ++i) {
            const link = links[i];
////        console.log('|' + link.href + '|');
////        console.log('|' + link.innerText + '|');
            clips.push({href: link.href, text: link.innerText});
        }
        dispatch(receiveClips(show, href, clips))
    })
}
