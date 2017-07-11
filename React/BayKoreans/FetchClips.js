import React from 'react';
import axios from 'axios';

import {ShowLink} from "./ShowLink";
import {urlViaCorsProxy} from "./Constants";

function listClips(obj)
{
////console.log('href: |' + obj.props.href + '|');
    const request = axios.get(urlViaCorsProxy(obj.props.href));
    request.then(response => {
      if (response.status !== 200) {
        console.error(response.status);
        console.error(response.statusText);
        console.error(response.headers);
      } else {
        let doc = new DOMParser().parseFromString(response.data, "text/html");
        // class="button red xLarge"
        // class="button blue xLarge"
        let links = doc.querySelectorAll("a[class^=button][class$=xLarge]");
////    console.log(links);
        let clips = [];
        for (let i = 0; i < links.length; ++i) {
            let link = links[i];
////        console.log('|' + link.href + '|');
////        console.log('|' + link.innerText + '|');
            clips.push({id: i, title: link.innerText, href: link.href});
        }
        obj.setState({'hide': false, 'clips': clips});
      }
    });
}

export class FetchClips extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            hide: true,
            clips: []
        };

        this.handleClick = this.handleClick.bind(this);
    }

    handleClick() {
////    console.log('FetchClips.handleClick: [' + this.props.id + ']: # of clips = ' + this.state.clips.length);
        if (this.state.clips.length === 0)
            listClips(this);
        else
            this.setState({hide: !this.state.hide});
    }

    componentWillReceiveProps(nextProps) {
////    console.log('FetchClips.componentWillReceiveProps: reset = ' + nextProps.reset);
        if (nextProps.reset) {
            this.setState({'hide': true, 'clips': []});
        }
    }

    render() {
////    console.log('FetchClips.render(): hide = ' + this.state.hide);
////    console.log('FetchClips.render(): # of clips = ' + this.state.clips.length);
        return (
          <div>
          <li key={this.props.id} onClick={this.handleClick}>{this.props.title}</li>
          {this.state.hide ? (
            null
          ) : (
          <ul>
            {this.state.clips.map((clip) =>
              <ShowLink key={clip.id} id={clip.id} title={clip.title} href={clip.href}/>
            )}
          </ul>
          )}
          </div>
        );
    }
}
