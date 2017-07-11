import React from 'react';

export class PageButton extends React.Component {
    constructor(props) {
        super(props);
        this.handleButtonClick = this.handleButtonClick.bind(this);
    }

    handleButtonClick() {
        if (!this.props.disabled) {
            this.props.onPageButtonClick(this.props.href);
            document.getElementById("autoFocused").focus();
        }
    }

    render() {
////  console.log('PageButton.render()');
      return (
        <button onClick={this.handleButtonClick}>{this.props.text}</button>
      );
    }
}
