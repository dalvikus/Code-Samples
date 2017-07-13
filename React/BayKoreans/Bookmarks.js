import {midsMap, bookmarkCookieKey} from './Constants';

function serializeBookmarks(bookmarks)
{
////console.log('serializeBookmarks');
    let arrayOfEncodedBookmarks = [];
    bookmarks.forEach((bookmark) => {
////    console.log('  mid: "' + bookmark[0] + '", title: "' + bookmark[1] + '"');
        arrayOfEncodedBookmarks.push(encodeURIComponent(
            encodeURIComponent(bookmark[0]) + '=' + encodeURIComponent(bookmark[1])
        ))
    });
    return encodeURIComponent(arrayOfEncodedBookmarks.join('&'));
}

function deserializeBookmarks(serializedBookmarks)
{
////console.log('deserializeBookmarks');
    let a = [];
    decodeURIComponent(serializedBookmarks).split('&').forEach((e) => {
////    console.log('  keyEqualValue: "' + e + '"');
        let keyValuePair = decodeURIComponent(e).split('=').map((e1) => decodeURIComponent(e1));
////    console.log('  key: "' + keyValuePair[0] + '", value: "' + keyValuePair[1] + '"');
        if (midsMap.hasOwnProperty(keyValuePair[0])) {
            if (keyValuePair[1] === '') {
                console.error('No title for mid: "' + keyValuePair[0] + '"');
            } else {
                a.push(keyValuePair);
            }
        } else {
            console.error('No mid: "' + keyValuePair[0] + '"');
        }
    });
    return a;
}

export function saveBookmarks(bookmarks)
{
////console.log('saveBookmarks');
    var serializedBookmarks = serializeBookmarks(bookmarks);
    let d = new Date()
    const oneYearInMilliseconds = 365 * 24 * 60 * 60 * 1000;
    d.setTime(d.getTime() + oneYearInMilliseconds);
    let strExpires = "expires=" + d.toGMTString();
    document.cookie = bookmarkCookieKey + '=' + serializedBookmarks + ";" + strExpires + ";path=/";
}

function deleteCookie(key)
{
////console.log('deleteCookie');
    if (key === '') {
        console.error('No key');
    } else {
////    console.log('  key: "' + key + '"');
        const strExpiresInPast = 'expires=Thu, 01 Jan 1970 00:00:01 GMT';
        document.cookie = key + '=;' + strExpiresInPast + ";path=/";
    }
}

function listAllCookies()
{
////console.log('listAllCookies');
    let cookies = document.cookie;
    if (cookies === '') {
        console.log('No cookie(s)');
        return [];
    }

    cookies.split('; ').forEach((keyEqualValue) => {
////    console.log('  keyEqualValue: "' + keyEqualValue + '"')
        let keyValuePair = keyEqualValue.split('=');
        console.log('  key: "' + keyValuePair[0] + '", value: "' + keyValuePair[1] + '"');
    });
}

function deleteAllCookies()
{
////console.log('deleteAllCookies');
    let cookies = document.cookie;
    if (cookies === '') {
        console.log('No cookie(s)');
        return [];
    }

    cookies.split('; ').forEach((keyEqualValue) => {
////    console.log('  keyEqualValue: "' + keyEqualValue + '"')
        let keyValuePair = keyEqualValue.split('=');
////    console.log('  key: "' + keyValuePair[0] + '", value: "' + keyValuePair[1] + '"');
        deleteCookie(keyValuePair[0]);
    });
}

export function loadBookmarks()
{
////console.log('loadBookmarks');
    let cookies = document.cookie;
    if (cookies === '') {
        console.log('No cookie(s)');
        return [];
    }

    var strBookmarkValue = '';
    cookies.split('; ').forEach((keyEqualValue) => {
////    console.log('keyEqualValue: "' + keyEqualValue + '"')
        let keyValuePair = keyEqualValue.split('=');
////    console.log('key: "' + keyValuePair[0] + '", value: "' + keyValuePair[1] + '"');
        if (keyValuePair[0] === bookmarkCookieKey)
            strBookmarkValue = keyValuePair[1];
    });
    if (strBookmarkValue === '') {
        console.error('No key: "' + bookmarkCookieKey + '"');
        return [];
    }
    return deserializeBookmarks(strBookmarkValue);
}

export function indexOf(bookmarks, mid, title) {
    let index = -1;
    for (let i = 0; i < bookmarks.length; ++i) {
        let bookmark = bookmarks[i];
        if (bookmark[0] === mid && bookmark[1] === title) {
            index = i;
            break;
        }
    };
    return index;
}
