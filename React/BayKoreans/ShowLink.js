import React from 'react';

export class ShowLink extends React.Component {
    constructor(props) {
        super(props);

        this.handleShowLinkClick = this.handleShowLinkClick.bind(this);
    }

    handleShowLinkClick() {
////    console.log('ShowLink.handleShowLinkClick: [' + this.props.id + ']: title = "' + this.props.title + '": href = "' + this.props.href + '"');
        window.open(this.props.href, "_blank");
    }

    render() {
        let href = this.props.href;
////    console.log('ShowLink.render(): href = "' + href + '"');
        return (
          href === '' ? null : <li className="clip-li" key={this.props.id} onClick={this.handleShowLinkClick}>{this.props.title}</li>
        );
    }
}
